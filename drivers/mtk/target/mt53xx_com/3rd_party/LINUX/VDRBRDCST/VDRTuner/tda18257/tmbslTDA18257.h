/*
  Copyright (C) 2010 NXP B.V., All Rights Reserved.
  This source code and any compilation or derivative thereof is the proprietary
  information of NXP B.V. and is confidential in nature. Under no circumstances
  is this software to be  exposed to or placed under an Open Source License of
  any type without the expressed written permission of NXP B.V.
 *
 * \file          tmbslTDA18257.h
 *
 * \date          %modify_time%
 *
 * \author        Alexandre TANT
 *
 * \brief         Describe briefly the purpose of this file.
 *
 * REFERENCE DOCUMENTS :
 *                TDA18257_Driver_User_Guide.pdf
 *
 * \section info Change Information
 *
*/

#ifndef _TMBSL_TDA18257_H
#define _TMBSL_TDA18257_H

#ifdef __cplusplus
extern "C"
{
#endif

/*============================================================================*/
/* TDA18257 Error Codes                                                       */
/*============================================================================*/

#define TDA18257_ERR_BASE                       (CID_COMP_TUNER | CID_LAYER_BSL)
#define TDA18257_ERR_COMP                       (CID_COMP_TUNER | CID_LAYER_BSL | TM_ERR_COMP_UNIQUE_START)

#define TDA18257_ERR_BAD_UNIT_NUMBER            (TDA18257_ERR_BASE + TM_ERR_BAD_UNIT_NUMBER)
#define TDA18257_ERR_ERR_NO_INSTANCES           (TDA18257_ERR_BASE + TM_ERR_NO_INSTANCES)
#define TDA18257_ERR_NOT_INITIALIZED            (TDA18257_ERR_BASE + TM_ERR_NOT_INITIALIZED)
#define TDA18257_ERR_ALREADY_SETUP              (TDA18257_ERR_BASE + TM_ERR_ALREADY_SETUP)
#define TDA18257_ERR_INIT_FAILED                (TDA18257_ERR_BASE + TM_ERR_INIT_FAILED)
#define TDA18257_ERR_BAD_PARAMETER              (TDA18257_ERR_BASE + TM_ERR_BAD_PARAMETER)
#define TDA18257_ERR_NOT_SUPPORTED              (TDA18257_ERR_BASE + TM_ERR_NOT_SUPPORTED)
#define TDA18257_ERR_NULL_CONTROLFUNC           (TDA18257_ERR_BASE + TM_ERR_NULL_CONTROLFUNC)
#define TDA18257_ERR_HW_FAILED                  (TDA18257_ERR_COMP + 0x0001)
#define TDA18257_ERR_NOT_READY                  (TDA18257_ERR_COMP + 0x0002)
#define TDA18257_ERR_BAD_VERSION                (TDA18257_ERR_COMP + 0x0003)
#define TDA18257_ERR_STD_NOT_SET                (TDA18257_ERR_COMP + 0x0004)
#define TDA18257_ERR_RF_NOT_SET                 (TDA18257_ERR_COMP + 0x0005)

/*============================================================================*/
/* Types and defines:                                                         */
/*============================================================================*/
#define TDA18257_DVBC_SUPPORTED
#define TDA18257_DVBT_SUPPORTED
#define TDA18257_DTMB_SUPPORTED
#define TDA18257_ISDBT_SUPPORTED
#define TDA18257_ANALOG_SUPPORTED
#define TDA18257_ATSC_SUPPORTED

#define TDA18257_CONFIG_STD_FREQ_NUM 8

typedef enum _TDA18257Version_t {
    TDA18257_VersionES1 = 0,                                       /* ES1 */
    TDA18257_VersionES2,                                           /* ES2 */
    TDA18257_VersionES3,                                           /* ES3 */
    TDA18257_VersionES4,                                           /* ES4 */
    TDA18257_VersionMax
} TDA18257Version_t, *pTDA18257Version_t;

typedef struct _TDA18257_AdditionnalData_t
{
    tmUnitSelect_t tOtherUnit; /* tUnit of the instance containing the tUnit of the other path */
}TDA18257_AdditionnalData_t;

typedef enum _TDA18257PowerState_t {
    TDA18257_PowerNormalMode = 0,                                 /* Device normal mode */
    TDA18257_PowerStandbyWithXtalOn,                              /* Device standby mode with Xtal Output */
    TDA18257_PowerStandbyWithLtOnWithXtalOn,                      /* Device standby mode with LT on and with Xtal Output */
    TDA18257_PowerStandbyWithPllOnWithXtalOn,                     /* Device standby mode with PLL on and with Xtal Output */
    TDA18257_PowerStandbyWithLtOnWithPllOnWithXtalOn,             /* Device standby mode with LT on with PLL on and with Xtal Output */
    TDA18257_PowerStandbySmoothLTOnXtOutOn,
    TDA18257_PowerStandbySmoothLTOffXtOutOff,
    TDA18257_PowerStandbySmoothLTOffLNAOnXtOutOn,
    TDA18257_PowerMax
} TDA18257PowerState_t, *pTDA18257PowerState_t;

typedef enum  _TDA18257ModeInUse_t {
	TDA18257_Mode_Single = 0,
	TDA18257_Mode_Dual = 1,
	TDA18257_ModeMax = 3
} TDA18257ModeInUse_t, *pTDA18257ModeInUse_t;

typedef enum _TDA18257StandardMode_t {
    TDA18257_StandardMode_Unknown = 0,                  /* Unknown standard */
#ifdef TDA18257_DVBC_SUPPORTED
    TDA18257_QAM_6MHz,                                  /* Digital TV QAM 6MHz */
    TDA18257_QAM_8MHz,                                  /* Digital TV QAM 8MHz */
#endif
#ifdef TDA18257_DVBT_SUPPORTED
    TDA18257_DVBT_1_7MHz,                               /* Digital TV DVB-T/T2 6MHz */
    TDA18257_DVBT_6MHz,                                 /* Digital TV DVB-T/T2 6MHz */
    TDA18257_DVBT_7MHz,                                 /* Digital TV DVB-T/T2 7MHz */
    TDA18257_DVBT_8MHz,                                 /* Digital TV DVB-T/T2 8MHz */
    TDA18257_DVBT_10MHz,                                /* Digital TV DVB-T/T2 10MHz */
#endif
#ifdef TDA18257_DTMB_SUPPORTED
    TDA18257_DTMB_8MHz,                                 /* Digital TV DMB-T 8MHz */
#endif
#ifdef TDA18257_ISDBT_SUPPORTED
    TDA18257_ISDBT_6MHz,                                /* Digital TV ISDB-T 6MHz*/
#endif
#ifdef TDA18257_ANALOG_SUPPORTED
    TDA18257_FM_Radio,                                  /* Analog FM Radio */
    TDA18257_ANLG_MN,                                   /* Analog TV M/N */
    TDA18257_ANLG_B,                                    /* Analog TV B */
    TDA18257_ANLG_GH,                                   /* Analog TV G/H */
    TDA18257_ANLG_I,                                    /* Analog TV I */
    TDA18257_ANLG_DK,                                   /* Analog TV D/K */
    TDA18257_ANLG_L,                                    /* Analog TV L */
    TDA18257_ANLG_LL,                                   /* Analog TV L' */
    TDA18257_Scanning,                                  /* Analog Preset Blind Scanning */
#endif
#ifdef TDA18257_ATSC_SUPPORTED
    TDA18257_ATSC_6MHz,
#endif
    TDA18257_StandardMode_Max
} TDA18257StandardMode_t, *pTDA18257StandardMode_t;

/* Register Bit-Field Definition */
typedef struct _TDA18257_BitField_t
{
    UInt8   Address;
    UInt8   PositionInBits;
    UInt8   WidthInBits;
    UInt8   Attributes;
}
TDA18257_BitField_t, *pTDA18257_BitField_t;

typedef enum _TDA18257XtalFreq_t {
	TDA18257_XtalFreq_Unknown,
	TDA18257_XtalFreq_16000000,
	TDA18257_XtalFreq_24000000,
	TDA18257_XtalFreq_25000000,
	TDA18257_XtalFreq_27000000,
	TDA18257_XtalFreq_30000000
} TDA18257XtalFreq_t, *pTDA18257XtalFreq_t;

typedef enum _TDA18257IF_Output_Level_t {
    TDA18257_IF_Output_Level_2Vpp_0_30dB = 0,           /* 2Vpp       0 - 30dB      */
    TDA18257_IF_Output_Level_1_25Vpp_min_4_26dB,        /* 1.25Vpp   -4 - 26dB      */
    TDA18257_IF_Output_Level_1Vpp_min_6_24dB,           /* 1Vpp      -6 - 24dB      */
    TDA18257_IF_Output_Level_0_8Vpp_min_8_22dB,         /* 0.8Vpp    -8 - 22dB      */
    TDA18257_IF_Output_Level_0_85Vpp_min_7_5_22_5dB,    /* 0.85Vpp   -7.5 - 22.5dB  */
    TDA18257_IF_Output_Level_0_7Vpp_min_9_21dB,         /* 0.7Vpp    -9 - 21dB      */
    TDA18257_IF_Output_Level_0_6Vpp_min_10_3_19_7dB,    /* 0.6Vpp    -10.3 - 19.7dB */
    TDA18257_IF_Output_Level_0_5Vpp_min_12_18dB,        /* 0.5Vpp    -12 - 18dB     */
    TDA18257_IF_Output_Level_Max
} TDA18257IF_Output_Level_t, *pTDA18257IF_Output_Level_t;

typedef enum _TDA18257Scanning_Mode_t {
    TDA18257_Scanning_Mode_Disabled = 0,
    TDA18257_Scanning_Mode_Enabled,
    TDA18257_Scanning_Mode_Max
} TDA18257Scanning_Mode_t, *pTDA18257Scanning_Mode_t;

/*============================================================================*/
/* Exported functions:                                                        */
/*============================================================================*/

tmErrorCode_t
tmbslTDA18257_Open(
    tmUnitSelect_t              tUnit,      /*  I: Unit number */
    tmbslFrontEndDependency_t*  psSrvFunc   /*  I: setup parameters */
);

tmErrorCode_t
tmbslTDA18257_HwInit(
    tmUnitSelect_t tUnit    /* I: Unit number */
);

tmErrorCode_t
tmbslTDA18257_SetPowerState(
    tmUnitSelect_t  tUnit,      /* I: Unit number */
    tmPowerState_t  powerState  /* I: Power state */
);

tmErrorCode_t
tmbslTDA18257_SetStandardMode(
    tmUnitSelect_t              tUnit,          /*  I: Unit number */
    TDA18257StandardMode_t    StandardMode    /*  I: Standard mode of this device */
);

tmErrorCode_t
tmbslTDA18257_SetRF(
    tmUnitSelect_t  tUnit,  /*  I: Unit number */
    UInt32          uRF     /*  I: RF frequency in hertz */
);

tmErrorCode_t
tmbslTDA18257_Close(
    tmUnitSelect_t  tUnit   /*  I: Unit number */
);

tmErrorCode_t
tmbslTDA18257_GetIF(
    tmUnitSelect_t  tUnit,  /* I: Unit number */
    UInt32*         puIF    /* O: IF Frequency in hertz */
);

tmErrorCode_t
tmbslTDA18257_SetIFLevel(
    tmUnitSelect_t              tUnit,      /* I: Unit number */
    TDA18257IF_Output_Level_t  eIFLevel    /* I: IF Level */
);

tmErrorCode_t
tmbslTDA18257_GetIFLevel(
    tmUnitSelect_t              tUnit,      /* I: Unit number */
    TDA18257IF_Output_Level_t  *peIFLevel  /* I: IF Level */
);

tmErrorCode_t
tmbslTDA18257_GetPowerLevel(
    tmUnitSelect_t  tUnit,      /* I: Unit number */
    Int32*          pPowerLevel /* O: Power Level in (x100) dBµVrms */
);

tmErrorCode_t
tmbslTDA18257_CheckHWVersion(
    tmUnitSelect_t  tUnit,      /* I: Unit number */
    pTDA18257Version_t pEsVersion  /*  o: ES version of the HW */
);

#ifdef __cplusplus
}
#endif

#endif /* _TMBSL_TDA18257_H */

