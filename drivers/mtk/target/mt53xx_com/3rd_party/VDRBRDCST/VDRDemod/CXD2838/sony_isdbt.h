/*------------------------------------------------------------------------------
  Copyright 2012 Sony Corporation

  Last Updated  : 2012-05-22
  File Revision : 1.0.0.0
------------------------------------------------------------------------------*/

#ifndef SONY_ISDBT_H
#define SONY_ISDBT_H

#include "sony_common.h"
#include "sony_i2c.h"

/*------------------------------------------------------------------------------
  Enumeration
------------------------------------------------------------------------------*/

typedef enum sony_isdbt_mode_t{
    SONY_ISDBT_MODE_1,    /*ISDB-T Mode 1(Number of carriers = 1405*/
    SONY_ISDBT_MODE_2,    /*ISDB-T Mode 2(Number of carriers = 2809*/
    SONY_ISDBT_MODE_3,    /*ISDB-T Mode 1(Number of carriers = 5617*/
    SONY_ISDBT_MODE_UNKNOWN
}sony_isdbt_mode_t;


typedef enum sony_isdbt_guard_t{
    SONY_ISDBT_GUARD_1_32, /* GI = 1/32 */
    SONY_ISDBT_GUARD_1_16, /* GI = 1/16 */
    SONY_ISDBT_GUARD_1_8,  /* GI = 1/8  */
    SONY_ISDBT_GUARD_1_4,  /* GI = 1/4  */
    SONY_ISDBT_GUARD_UNKNOWN
}sony_isdbt_guard_t;


/* ===== TMCC Information ===== */

typedef enum sony_isdbt_tmcc_system_t{
    SONY_ISDBT_TMCC_SYSTEM_ISDB_T,      /* ISDB for Terrestrial Television Broadcasting */
    SONY_ISDBT_TMCC_SYSTEM_ISDB_TSB,    /* ISDB for Terrestrial Sound Broadcasting      */
    SONY_ISDBT_TMCC_SYSTEM_RESERVE_2,
    SONY_ISDBT_TMCC_SYSTEM_RESERVE_3
}sony_isdbt_tmcc_system_t;


typedef enum sony_isdbt_tmcc_modulation_t{
    SONY_ISDBT_TMCC_MODULATION_DQPSK,        /* Modulation Method : DQPSK */
    SONY_ISDBT_TMCC_MODULATION_QPSK,         /* Modulation Method : QPSK  */
    SONY_ISDBT_TMCC_MODULATION_16QAM,        /* Modulation Method : 16QAM */
    SONY_ISDBT_TMCC_MODULATION_64QAM,        /* Modulation Method : 64QAM */
    SONY_ISDBT_TMCC_MODULATION_RESERVED_4,
    SONY_ISDBT_TMCC_MODULATION_RESERVED_5,
    SONY_ISDBT_TMCC_MODULATION_RESERVED_6,
    SONY_ISDBT_TMCC_MODULATION_UNUSED_7
}sony_isdbt_tmcc_modulation_t;


typedef enum sony_isdbt_tmcc_coding_rate_t{
    SONY_ISDBT_TMCC_CODING_RATE_1_2,        /* 1/2 */
    SONY_ISDBT_TMCC_CODING_RATE_2_3,        /* 2/3 */
    SONY_ISDBT_TMCC_CODING_RATE_3_4,        /* 3/4 */
    SONY_ISDBT_TMCC_CODING_RATE_5_6,        /* 5/6 */
    SONY_ISDBT_TMCC_CODING_RATE_7_8,        /* 7/8 */
    SONY_ISDBT_TMCC_CODING_RATE_RESERVED_5,
    SONY_ISDBT_TMCC_CODING_RATE_RESERVED_6,
    SONY_ISDBT_TMCC_CODING_RATE_UNUSED_7
}sony_isdbt_tmcc_coding_rate_t;


typedef enum sony_isdbt_tmcc_il_length_t{
    SONY_ISDBT_TMCC_IL_LENGTH_0_0_0,      /* Mode1: 0, Mode2: 0, Mode3: 0 */
    SONY_ISDBT_TMCC_IL_LENGTH_4_2_1,      /* Mode1: 4, Mode2: 2, Mode3: 1 */
    SONY_ISDBT_TMCC_IL_LENGTH_8_4_2,      /* Mode1: 8, Mode2: 4, Mode3: 2 */
    SONY_ISDBT_TMCC_IL_LENGTH_16_8_4,     /* Mode1:16, Mode2: 8, Mode3: 4 */
    SONY_ISDBT_TMCC_IL_LENGTH_RESERVED_4,
    SONY_ISDBT_TMCC_IL_LENGTH_RESERVED_5,
    SONY_ISDBT_TMCC_IL_LENGTH_RESERVED_6,
    SONY_ISDBT_TMCC_IL_LENGTH_UNUSED_7
}sony_isdbt_tmcc_il_length_t;


/*------------------------------------------------------------------------------
  Struct
------------------------------------------------------------------------------*/

typedef struct sony_isdbt_tmcc_layer_info_t{
    sony_isdbt_tmcc_modulation_t  modulation; /* Modulation */
    sony_isdbt_tmcc_coding_rate_t codingRate; /* Coding rate */
    sony_isdbt_tmcc_il_length_t   ilLength;   /* Interleaving length */
    UINT8 segmentsNum; /* 1-13:Seguments number, 0,14:Reserved, 15:Unused */
}sony_isdbt_tmcc_layer_info_t;


typedef struct sony_isdbt_tmcc_group_param_t{
    UINT8 isPartial; /* 0:No partial reception, 1:Partial receiption available */
    sony_isdbt_tmcc_layer_info_t layerA;
    sony_isdbt_tmcc_layer_info_t layerB;
    sony_isdbt_tmcc_layer_info_t layerC;
}sony_isdbt_tmcc_group_param_t;


typedef struct sony_isdbt_tmcc_info_t{
    sony_isdbt_tmcc_system_t      systemId;
    UINT8                       countDownIndex; /* 4bit 0x0F:normal, 0x0E - 0x00:(X+1)frames prior to switching */
    UINT8                       ewsFlag;        /* 1bit 0:Not start EWS, 1:Start EWS */
    sony_isdbt_tmcc_group_param_t currentInfo;
    sony_isdbt_tmcc_group_param_t nextInfo;
}sony_isdbt_tmcc_info_t;

typedef struct sony_isdbt_aceew_area_t {
    UINT8 data[11];
} sony_isdbt_aceew_area_t;

typedef struct sony_isdbt_demod_aceew_epicenter_t {
    UINT8 data[11];
} sony_isdbt_aceew_epicenter_t;

typedef struct sony_isdbt_aceew_info_t {
    UINT8 startEndFlag;                               /* B17-B18 in ARIB STD-B31 ACEEW information */
    UINT8 updateFlag;                                 /* B19-B20 in ARIB STD-B31 ACEEW information */
    UINT8 signalId;                                   /* B21-B23 in ARIB STD-B31 ACEEW information */

    /* Area information */
    UINT8 isAreaValid;                                /* areaInfo is (0: Invalid, 1:Valid) */
    sony_isdbt_aceew_area_t areaInfo;            /* Area information data (B24 - B111) */

    /* Epicenter1 information */
    UINT8 isEpicenter1Valid;                          /* epicenter1Info is (0: Invalid, 1:Valid) */
    sony_isdbt_aceew_epicenter_t epicenter1Info; /* Epicenter1 information data (B24 - B111) */

    /* Epicenter2 information */
    UINT8 isEpicenter2Valid;                          /* epicenter2Info is (0: Invalid, 1:Valid) */
    sony_isdbt_aceew_epicenter_t epicenter2Info; /* Epicenter2 information data (B24 - B111) */

} sony_isdbt_aceew_info_t;
#endif  /*SONY_ISDBT_H*/
