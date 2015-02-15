/*
  NOTE: This is glue layer code of ASCOT3HELENE driver but not perfect
  because Sony do not have much information about MTK software.
  Please modify it to fit MTK's system.
*/

#include "sonyrf_apmd_ascot3_module_forBRAVIA.h"
#include "sonyrf_apmd_helene_module_forBRAVIA.h"
#include "sonyrf_i2c_MTK.h"

#ifndef SONYRF_DUMMY_DEBUG // For checking driver in Sony dummy environment. Please ignore it.
#include "tuner_interface_if.h"
#include "fe_tuner_common_if.h"
#include "i2c_api.h"
#include "x_typedef.h"
#include "tunerDebug.h"
#include "x_os.h"
#include "eq_script_ascot3helene.h"
#include "pi_demod_common_def.h"
#include "cust_common_interface.h"
#else // SONYRF_DUMMY_DEBUG
#include "MTKdef.h"
#endif

//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------
#define C_ASCOT3HELENE_LO_ADDRESS           0xC0 // In 8bit form

#define C_ASCOT3HELENE_IF_FREQUENCY_DVBT_6M         ((UINT16)  3600)  /* kHz */  //JUST for 5.81M
#define C_ASCOT3HELENE_IF_FREQUENCY_DVBT_7M         ((UINT16)  4200)  /* kHz */  //JUST for 6.8M
#define C_ASCOT3HELENE_IF_FREQUENCY_DVBT_8M         ((UINT16)  4800)  /* kHz */  //JUST for 7.65M
#define C_ASCOT3HELENE_IF_FREQUENCY_DVBT2_1_7M      ((UINT16)  3500)  /* kHz */
#define C_ASCOT3HELENE_IF_FREQUENCY_DVBT2_6M        ((UINT16)  3600)  /* kHz */  //JUST for 6.8M
#define C_ASCOT3HELENE_IF_FREQUENCY_DVBT2_7M        ((UINT16)  4200)  /* kHz */  //JUST for 6.8M
#define C_ASCOT3HELENE_IF_FREQUENCY_DVBT2_8M        ((UINT16)  4800)  /* kHz */  //JUST for 7.65M
#define C_ASCOT3HELENE_IF_FREQUENCY_DVBC            ((UINT16)  4900)  /* kHz */  //JUST for 7.95M
#define C_ASCOT3HELENE_IF_FREQUENCY_DTMB            ((UINT16)  5100)  /* kHz */  //JUST for 7.95M
#define C_ASCOT3HELENE_IF_FREQUENCY_ATSC            ((UINT16)  3700)  /* kHz */
#define C_ASCOT3HELENE_IF_FREQUENCY_QAM             ((UINT16)  3700)  /* kHz */
#define C_ASCOT3HELENE_IF_FREQUENCY_DVBC2_6M        ((UINT16)  3700)  /* kHz */
#define C_ASCOT3HELENE_IF_FREQUENCY_DVBC2_8M        ((UINT16)  4900)  /* kHz */
#define C_ASCOT3HELENE_IF_FREQUENCY_ISDBT_6M        ((UINT16)  3550)  /* kHz */
#define C_ASCOT3HELENE_IF_FREQUENCY_ISDBT_7M        ((UINT16)  4150)  /* kHz */
#define C_ASCOT3HELENE_IF_FREQUENCY_ISDBT_8M        ((UINT16)  4750)  /* kHz */

#define C_ASCOT3HELENE_IF_FREQUENCY_PAL_BG          ((UINT16)  7300)  /* kHz */  //JUST for 7.95M
#define C_ASCOT3HELENE_IF_FREQUENCY_PAL_DK          ((UINT16)  7850)  /* kHz */  //JUST for 7.95M
#define C_ASCOT3HELENE_IF_FREQUENCY_PAL_I           ((UINT16)  7850)  /* kHz */  //JUST for 7.95M
#define C_ASCOT3HELENE_IF_FREQUENCY_SEMCAL_L        ((UINT16)  7850)  /* kHz */  //JUST for 7.95M
#define C_ASCOT3HELENE_IF_FREQUENCY_SEMCAL_L1       ((UINT16)  2200)  /* kHz */  //JUST for 7.95M
#define C_ASCOT3HELENE_IF_FREQUENCY_NTSC            ((UINT16)  5750)  /* kHz */  //JUST for 7.95M

#define C_ASCOT3HELENE_SAW_BW              SAW_BW_8M

//Frequency boundary .Get from sony spec
#define C_ASCOT3HELENE_FREQ_DBOUND_UPPER      870*1000*1000 //dtmb need 870
#define C_ASCOT3HELENE_FREQ_DBOUND_LOWER        47*1000*1000
#define C_ASCOT3HELENE_FREQ_DBOUND_UPPER_Ana  875*1000*1000
#define C_ASCOT3HELENE_FREQ_DBOUND_LOWER_Ana    42*1000*1000

#define C_ASCOT3HELENE_CHANNEL_SCAN_JUMP_SMALL_STEP ((UINT16) 250)
#define C_ASCOT3HELENE_CHANNEL_SCAN_JUMP_MIDDLE_STEP_UP ((UINT16) 1500)
#define C_ASCOT3HELENE_CHANNEL_SCAN_JUMP_MIDDLE_STEP_DOWN ((UINT16) 500)

#define C_SONYRF_ASCOT3HELENE_STR        "SONYRF_ASCOT3HELENE 2014-08-11\n v0.3"

#ifndef SONYRF_DUMMY_DEBUG

// DVB-T+DVB-C D-only Tuner
#define ccTUNER_LOCK_CONF_THRESHOLD     0
#define cALC_ADC_COMM_VOLT              1.2
#define cALC_ADC_INPUT_SWING            2  //Volt
#define cALC_ADC_BIAS                   (cALC_ADC_COMM_VOLT - cALC_ADC_INPUT_SWING/2)
//Begin_EU DTV PRA Setting
//AGC range 0~3.3V
#define C_ASCOT3HELENE_AGC_IF_SLP            -0.25//-(1/4)
#define C_ASCOT3HELENE_AGC_IF_INT           	0	//0
#define C_ASCOT3HELENE_AGC_IF_MAX           0.998  // 3.3
#define C_ASCOT3HELENE_AGC_IF_MIN          -1//0
#define C_ASCOT3HELENE_AGC_IF_SLP_SGN  (INT8)((C_ASCOT3HELENE_AGC_IF_SLP>0)?1:-1)
#define C_ASCOT3HELENE_AGC_IF_LVL_MAX      0.4774    //-90dBm    127/256
#define C_ASCOT3HELENE_AGC_IF_LVL_MIN      0.08203125   //-60dBm  21/256
//End_EU DTV PRA Setting
#define C_ASCOT3HELENE_MAX_IF_GAIN_SET       -0.12
#define C_ASCOT3HELENE_MIN_IF_GAIN_SET       -0.38
#define C_ASCOT3HELENE_MAX_RF_GAIN_SET        cALC_ADC_BIAS *2   // Volt
#define C_ASCOT3HELENE_MIN_RF_GAIN_SET        cALC_ADC_BIAS *2   // Volt
#define C_ASCOT3HELENE_MAX_IF_GAIN_POW       -100    // dBm
#define C_ASCOT3HELENE_MIN_IF_GAIN_POW     -50     // dBm
#define C_ASCOT3HELENE_MAX_RF_GAIN_POW      C_ASCOT3HELENE_MIN_IF_GAIN_POW
#define C_ASCOT3HELENE_MIN_RF_GAIN_POW     -20     // dBm
#define C_ASCOT3HELENE_POWER_CALIBRATE        26     // dBm
//#define C_ASCOT3HELENE_SIGNAL_LVL_TH          6  //liuqu,20080926,no use for nimtool

#define C_ASCOT3HELENE_MAX_IF_GAIN_SET       -0.12
#define C_ASCOT3HELENE_MIN_IF_GAIN_SET       -0.38
#define C_ASCOT3HELENE_MAX_RF_GAIN_SET        cALC_ADC_BIAS *2   // Volt
#define C_ASCOT3HELENE_MIN_RF_GAIN_SET        cALC_ADC_BIAS *2   // Volt
#define C_ASCOT3HELENE_MAX_IF_GAIN_POW       -100    // dBm
#define C_ASCOT3HELENE_MIN_IF_GAIN_POW     -50     // dBm
#define C_ASCOT3HELENE_MAX_RF_GAIN_POW      C_ASCOT3HELENE_MIN_IF_GAIN_POW
#define C_ASCOT3HELENE_MIN_RF_GAIN_POW     -20     // dBm
#define C_ASCOT3HELENE_POWER_CALIBRATE        26     // dBm

#define C_ASCOT3HELENE_AGC_COEF            ((UINT8)   0xBD)
//#define C_ASCOT3HELENE_DEMOD_INPUT_POWER cIF_TARGET_LEVEL_0CCC_0D36

#define C_UINT8_ASCOT3HELENE_AGC_IF_SLP         (INT8)(C_ASCOT3HELENE_AGC_IF_SLP *64 )  // MT5135(S(1, -6)), -0.125*64 = -8 (0xF8), -8/64 = -1/8
#define C_UINT8_ASCOT3HELENE_AGC_IF_INT         (INT8)(C_ASCOT3HELENE_AGC_IF_INT *16 ) // MT5135(S(3, -4))  0.5*16 = 8, 8/16 =0.5
#define C_UINT8_ASCOT3HELENE_AGC_IF_MAX         (INT8)(C_ASCOT3HELENE_AGC_IF_MAX *128)    //0.499*256 = 0x7F
#define C_UINT8_ASCOT3HELENE_AGC_IF_MIN         (INT8)(C_ASCOT3HELENE_AGC_IF_MIN *128)    //0
#define C_UINT8_ASCOT3HELENE_AGC_IF_LVL_MAX        (INT8)(C_ASCOT3HELENE_AGC_IF_LVL_MAX *256)
#define C_UINT8_ASCOT3HELENE_AGC_IF_LVL_MIN         (INT8)(C_ASCOT3HELENE_AGC_IF_LVL_MIN *256)

#define C_UINT8_ASCOT3HELENE_MAX_IF_GAIN_SET   (INT8)(C_ASCOT3HELENE_MAX_IF_GAIN_SET *256)
#define C_UINT8_ASCOT3HELENE_MIN_IF_GAIN_SET   (INT8)(C_ASCOT3HELENE_MIN_IF_GAIN_SET *256)
#define C_UINT8_ASCOT3HELENE_MAX_RF_GAIN_SET   (INT8)((C_ASCOT3HELENE_MAX_RF_GAIN_SET /2 -cALC_ADC_BIAS) /cALC_ADC_INPUT_SWING *256)
#define C_UINT8_ASCOT3HELENE_MIN_RF_GAIN_SET   (INT8)((C_ASCOT3HELENE_MIN_RF_GAIN_SET /2 -cALC_ADC_BIAS) /cALC_ADC_INPUT_SWING *256)
#define C_UINT8_ASCOT3HELENE_IF_GAIN_RANGE      (INT8)(C_ASCOT3HELENE_MIN_IF_GAIN_POW - C_ASCOT3HELENE_MAX_IF_GAIN_POW)
#define C_UINT8_ASCOT3HELENE_RF_GAIN_RANGE      (INT8)(C_ASCOT3HELENE_MIN_RF_GAIN_POW - C_ASCOT3HELENE_MAX_RF_GAIN_POW)

static UINT8* pASCOT3HELENEEqNormal[MOD_ANA_TYPE_END - MOD_ANA_TYPE_BEGIN] = {NULL};
static UINT8* pASCOT3HELENEEqWeak[MOD_ANA_TYPE_END - MOD_ANA_TYPE_BEGIN] = {NULL};
#define PRA_TARGET_H_INDEX 0
#define PRA_TARGET_L_INDEX 1
#define SLD_DAGC_00_INDEX 2
#define DAGC_TARGET_LEVEL_H_INDEX 4
#define DAGC_TARGET_LEVEL_L_INDEX 5
#define PRA_LOOP_CONF_INDEX  6

extern UCHAR u1Rsetting;   // >=1:have IF attenuator, 0:No IF attenuator

static UINT8 initTimes = 0;

#define SONY_DEMOD_IIC_ADDRESS_0  0xD8
#define SONY_DEMOD_IIC_ADDRESS_1  0xDA

//Tuner releated ATD register
static ATD_TUNER_REL_REG_T arrAtdRelReg[]={
#if !(defined(CC_MT5365) || defined(CC_MT5395))
     {0x7b7,0x10}, //PRA target H
     {0x7b9,0x10}, //PRA target L
     {0x790,0x08}, //DAGC setting
     {0x7df,0x51}, //AGC polarity inverse.RF_AGC    disable.IF_AGC polarity inverse
     {0x794,0x2c}, //DAGC Target level H
     {0x7a4,0x2c}, //DAGC Target level L
     {0x7ca,0x88}, //PRA speed
     {0x7d4,0x00},//RF_BIAS  /*wenming 11-12-07: new PRA parameter*/
     {0x7d5,0x00},//IF_BIAS1
     {0x7d6,0x00},//IF_BIAS2
     {0x7d7,0x00},//RF_SLOPE
     {0x7d8,0xf0},//IF_SLOPE1
     {0x7d9,0xf0},//IF_SLOPE2
     {0x7da,0x7f},//RF Max
     {0x7db,0x7f},//IF Max
     {0x7dc,0x80},//RF Min
     {0x7dd,0x80},//IF Min 1
     {0x7de,0x80},//IF Min 2
     {0x00,0x00}//end flag
#else
    {0x2B7,0x10},//PRA STATIC TARGET LEVEL HIGH(MSB),degrade target level for lowif tuner(legacy)
    {0x2B9,0x10},//PRA STATIC TARGET LEVEL LOW(MSB)
    {0x290,0x08},//DAGC DC REMOVER CONFIGURATION
    {0x2df,0x51},
    {0x294,0x2c}, //DAGC Target H
    {0x2a4,0x2c}, //DAGC Target L
    {0x2ca,0x88}, //PRA speed
    {0x2D4,0x04},//PRA RF BIAS
    {0x2D5,0x04},//PRA IF BIAS 1
    {0x2D6,0x04},//PRA IF BIAS 2
    {0x2D7,0xEC},//PRA RF SCLOPE
    {0x2D8,0xF4},//PRA IF SLOPE 1
    {0x2D9,0xF4},//PRA IF SLOPE 2
    {0x2DA,0x7F},//PRA RF AGC MAXIMUM
    {0x2DB,0x7F},//PRA IF AGC MINIMUM
    {0x2DC,0x80},//PAR RF AGC MINIMUM
    {0x2DD,0x80},//PRA IF AGC MINIMUM 1
    {0x2DE,0x80},//PRA IF AGC MINIMUM 2
    {0x00,0x00}//end flag
#endif
};
#endif // SONYRF_DUMMY_DEBUG

/* ----------------------------------------------------------------------------------------------- */
/* Tuner Driver data type. */
/* NOTE: ALL tuner IC instance related data should be stored in this struct to support dual tuner. */
typedef enum {
    TUNERIC_ASCOT3, // ASCOT3/ASCOT3I
    TUNERIC_HELENE
} TUNERIC_T;

typedef struct sonyrf_tuner_ctx
{
    union {
        sonyrf_ascot3_t ascot3; // ASCOT3/ASCOT3I
        sonyrf_helene_t helene; // HELENE
    } tuner;
    sonyrf_i2c_t     i2c;

    BOOL fgCable;    /* Analog signal input, 0:Air input, 1:Cable input */

    TUNERIC_T tunerIcType; // 0 : ASCOT3/ASCOT3I, 1 : HELENE
    BOOL tunerInited;
} sonyrf_tuner_ctx;

/* Tuner control data instance. */
#define TUNERCTX_NUM 2
static sonyrf_tuner_ctx g_sonyrf_tuner_ctx[TUNERCTX_NUM]; /* Index 0 : Main, 1 : Sub */
/* Sony Product Information */
static SONYPRODUCT_TYPEINFO g_sonyproduct_typeinfo;

/* This table is used ONLY for SCT to control IF output */
#define AUTO         (0xFF) /* For IF_OUT_SEL and AGC_SEL, it means that the value is desided by config flags. */
                            /* For RF_GAIN, it means that RF_GAIN_SEL(SubAddr:0x4E) = 1 */
#define OFFSET(ofs)  ((uint8_t)(ofs) & 0x1F)
#define BW_6         (0x00)
#define BW_7         (0x01)
#define BW_8         (0x02)
#define BW_1_7       (0x03)

#define PORT1        (0x00)
#define PORT2        (0x01)

static sonyrf_ascot3_adjust_param_t g_param_table_SCT[SONYRF_ASCOT3_TV_SYSTEM_NUM] = {
    /*
    OUTLMT    IF_BPF_GC                                           BW              BW_OFFSET         IF_OUT_SEL
      |  RF_GAIN  |     RFOVLD_DET_LV1    IFOVLD_DET_LV  IF_BPF_F0 |   FIF_OFFSET     |        AGC_SEL |  IS_LOWERLOCAL
      |     |     |    (VL)  (VH)  (U)   (VL)  (VH)  (U)    |      |       |          |          |     |     |          */
    {0x00, AUTO, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, BW_6,  OFFSET(0),  OFFSET(0),  AUTO, AUTO, 0x00}, /**< SONYRF_ASCOT3_TV_SYSTEM_UNKNOWN */
    /* Analog */
    {0x00, AUTO, 0x05, 0x02, 0x05, 0x02, 0x01, 0x01, 0x01, 0x00, BW_6,  OFFSET(0),  OFFSET(1),  PORT1, PORT1, 0x00}, /**< SONYRF_ASCOT3_ATV_MN_EIAJ   (System-M (Japan)) */
    {0x00, AUTO, 0x05, 0x02, 0x05, 0x02, 0x01, 0x01, 0x01, 0x00, BW_6,  OFFSET(0),  OFFSET(1),  PORT1, PORT1, 0x00}, /**< SONYRF_ASCOT3_ATV_MN_SAP    (System-M (US)) */
    {0x00, AUTO, 0x05, 0x02, 0x05, 0x02, 0x01, 0x01, 0x01, 0x00, BW_6,  OFFSET(3),  OFFSET(1),  PORT1, PORT1, 0x00}, /**< SONYRF_ASCOT3_ATV_MN_A2     (System-M (Korea)) */
    {0x00, AUTO, 0x05, 0x02, 0x05, 0x02, 0x01, 0x01, 0x01, 0x00, BW_7,  OFFSET(11), OFFSET(5),  PORT1, PORT1, 0x00}, /**< SONYRF_ASCOT3_ATV_BG        (System-B/G) */
    {0x00, AUTO, 0x05, 0x02, 0x05, 0x02, 0x01, 0x01, 0x01, 0x00, BW_8,  OFFSET(2),  OFFSET(-3), PORT1, PORT1, 0x00}, /**< SONYRF_ASCOT3_ATV_I         (System-I) */
    {0x00, AUTO, 0x05, 0x02, 0x05, 0x02, 0x01, 0x01, 0x01, 0x00, BW_8,  OFFSET(2),  OFFSET(-3), PORT1, PORT1, 0x00}, /**< SONYRF_ASCOT3_ATV_DK        (System-D/K) */
    {0x00, AUTO, 0x03, 0x03, 0x06, 0x03, 0x04, 0x04, 0x04, 0x00, BW_8,  OFFSET(2),  OFFSET(-3), PORT1, PORT1, 0x00}, /**< SONYRF_ASCOT3_ATV_L         (System-L) */
    {0x00, AUTO, 0x03, 0x03, 0x06, 0x03, 0x04, 0x04, 0x04, 0x00, BW_8,  OFFSET(-1), OFFSET(4),  PORT1, PORT1, 0x00}, /**< SONYRF_ASCOT3_ATV_L_DASH    (System-L DASH) */
    /* Digital */
    {0x00, AUTO, 0x09, 0x0C, 0x0C, 0x0C, 0x03, 0x03, 0x03, 0x00, BW_6,  OFFSET(-6), OFFSET(-3), PORT1, PORT1, 0x00}, /**< SONYRF_ASCOT3_DTV_8VSB      (ATSC 8VSB) */
    {0x00, AUTO, 0x09, 0x0C, 0x0C, 0x0C, 0x02, 0x02, 0x02, 0x00, BW_6,  OFFSET(-6), OFFSET(-3), PORT1, PORT1, 0x00}, /**< SONYRF_ASCOT3_DTV_QAM       (US QAM) */
    {0x00, AUTO, 0x09, 0x0C, 0x0C, 0x0C, 0x02, 0x02, 0x02, 0x00, BW_6,  OFFSET(-9), OFFSET(-5), PORT2, PORT2, 0x00}, /**< SONYRF_ASCOT3_DTV_ISDBT_6   (ISDB-T 6MHzBW) */
    {0x00, AUTO, 0x09, 0x0C, 0x0C, 0x0C, 0x02, 0x02, 0x02, 0x00, BW_7,  OFFSET(-7), OFFSET(-6), PORT2, PORT2, 0x00}, /**< SONYRF_ASCOT3_DTV_ISDBT_7   (ISDB-T 7MHzBW) */
    {0x00, AUTO, 0x09, 0x0C, 0x0C, 0x0C, 0x02, 0x02, 0x02, 0x00, BW_8,  OFFSET(-5), OFFSET(-7), PORT2, PORT2, 0x00}, /**< SONYRF_ASCOT3_DTV_ISDBT_8   (ISDB-T 8MHzBW) */
    {0x00, AUTO, 0x09, 0x0C, 0x0C, 0x0C, 0x02, 0x02, 0x02, 0x00, BW_6,  OFFSET(-8), OFFSET(-3), PORT2, PORT2, 0x00}, /**< SONYRF_ASCOT3_DTV_DVBT_5    (DVB-T 5MHzBW) */
    {0x00, AUTO, 0x09, 0x0C, 0x0C, 0x0C, 0x02, 0x02, 0x02, 0x00, BW_6,  OFFSET(-8), OFFSET(-3), PORT2, PORT2, 0x00}, /**< SONYRF_ASCOT3_DTV_DVBT_6    (DVB-T 6MHzBW) */
    {0x00, AUTO, 0x09, 0x0C, 0x0C, 0x0C, 0x02, 0x02, 0x02, 0x00, BW_7,  OFFSET(-6), OFFSET(-5), PORT2, PORT2, 0x00}, /**< SONYRF_ASCOT3_DTV_DVBT_7    (DVB-T 7MHzBW) */
    {0x00, AUTO, 0x09, 0x0C, 0x0C, 0x0C, 0x02, 0x02, 0x02, 0x00, BW_8,  OFFSET(-4), OFFSET(-6), PORT2, PORT2, 0x00}, /**< SONYRF_ASCOT3_DTV_DVBT_8    (DVB-T 8MHzBW) */
    {0x00, AUTO, 0x09, 0x0C, 0x0C, 0x0C, 0x02, 0x02, 0x02, 0x00, BW_1_7,OFFSET(-10),OFFSET(-10),PORT2, PORT2, 0x00}, /**< SONYRF_ASCOT3_DTV_DVBT2_1_7 (DVB-T2 1.7MHzBW) */
    {0x00, AUTO, 0x09, 0x0C, 0x0C, 0x0C, 0x02, 0x02, 0x02, 0x00, BW_6,  OFFSET(-8), OFFSET(-3), PORT2, PORT2, 0x00}, /**< SONYRF_ASCOT3_DTV_DVBT2_5   (DVB-T2 5MHzBW) */
    {0x00, AUTO, 0x09, 0x0C, 0x0C, 0x0C, 0x02, 0x02, 0x02, 0x00, BW_6,  OFFSET(-8), OFFSET(-3), PORT2, PORT2, 0x00}, /**< SONYRF_ASCOT3_DTV_DVBT2_6   (DVB-T2 6MHzBW) */
    {0x00, AUTO, 0x09, 0x0C, 0x0C, 0x0C, 0x02, 0x02, 0x02, 0x00, BW_7,  OFFSET(-6), OFFSET(-5), PORT2, PORT2, 0x00}, /**< SONYRF_ASCOT3_DTV_DVBT2_7   (DVB-T2 7MHzBW) */
    {0x00, AUTO, 0x09, 0x0C, 0x0C, 0x0C, 0x02, 0x02, 0x02, 0x00, BW_8,  OFFSET(-4), OFFSET(-6), PORT2, PORT2, 0x00}, /**< SONYRF_ASCOT3_DTV_DVBT2_8   (DVB-T2 8MHzBW) */
    {0x00, AUTO, 0x05, 0x09, 0x09, 0x09, 0x02, 0x02, 0x02, 0x00, BW_6,  OFFSET(-6), OFFSET(-4), PORT2, PORT2, 0x00}, /**< SONYRF_ASCOT3_DTV_DVBC_6    (DVB-C 6MHzBW) */
    {0x00, AUTO, 0x05, 0x09, 0x09, 0x09, 0x02, 0x02, 0x02, 0x00, BW_8,  OFFSET(-2), OFFSET(-3), PORT2, PORT2, 0x00}, /**< SONYRF_ASCOT3_DTV_DVBC_8    (DVB-C 8MHzBW) */
    {0x00, AUTO, 0x03, 0x0A, 0x0A, 0x0A, 0x02, 0x02, 0x02, 0x00, BW_6,  OFFSET(-6), OFFSET(-2), PORT2, PORT2, 0x00}, /**< SONYRF_ASCOT3_DTV_DVBC2_6   (DVB-C2 6MHzBW) */
    {0x00, AUTO, 0x03, 0x0A, 0x0A, 0x0A, 0x02, 0x02, 0x02, 0x00, BW_8,  OFFSET(-2), OFFSET(0),  PORT2, PORT2, 0x00}, /**< SONYRF_ASCOT3_DTV_DVBC2_8   (DVB-C2 8MHzBW) */
    {0x00, AUTO, 0x04, 0x0C, 0x0C, 0x0C, 0x02, 0x02, 0x02, 0x00, BW_8,  OFFSET(2),  OFFSET(1),  PORT2, PORT2, 0x00}  /**< SONYRF_ASCOT3_DTV_DTMB      (DTMB) */
};

// Prototype
void ASCOT3HELENE_SetTerrLNA(UINT8 tunerID, ITUNER_CTX_T * pTunerCtx, UINT8 setVal);

//-----------------------------------------------------------------------------
/** ASCOT3HELENE_TunerInit
 *  Tuner initialzation for ASCOT3HELENE.
 *  @param  pTCtx       Pointer of pi tuner driver context.
 *  @retval void
 */
//-----------------------------------------------------------------------------
static VOID ASCOT3HELENE_TunerInit(UINT8 tunerID, ITUNER_CTX_T *pTCtx)
{
#ifndef SONYRF_DUMMY_DEBUG
    SPECIFIC_MEMBER_US_CTX * pUSCtx= &(pTCtx->specific_member.us_ctx);
    SPECIFIC_MEMBER_EU_CTX * pEUCtx= &(pTCtx->specific_member.eu_ctx);
    ITUNER_CTX_T *psTunerCtx = pTCtx;
#endif // SONYRF_DUMMY_DEBUG

    initTimes++;
    VDRBRDCST_GetSonyProductTypeInfo(&g_sonyproduct_typeinfo);
    mcDBG_MSG_L2(("ASCOT3HELENETuner[%d] : Board Ver = %d, Model Variation ID = %d\n", tunerID,
        g_sonyproduct_typeinfo.eBoardVer, g_sonyproduct_typeinfo.modelVariationID));

#ifndef SONYRF_DUMMY_DEBUG

    psTunerCtx->I2cAddress =C_ASCOT3HELENE_LO_ADDRESS;
    psTunerCtx->u2IF_Freq = C_ASCOT3HELENE_IF_FREQUENCY_PAL_BG;
    psTunerCtx->u2IF_Freq_A = C_ASCOT3HELENE_IF_FREQUENCY_DVBC;//analog if frequency for ATD


    //ATD patch selection
    psTunerCtx->u1AtdPatchSwitch =0x09;
    psTunerCtx->fgRFAGC = FALSE;//indicate if tuner need extern RF_AGC
    psTunerCtx->fgRFTuner =FALSE;//not sawless tuner,it include BPF internal
    // AGC para
    psTunerCtx->sAtdAgcPara.AgcRfBias = 0x2F;
    psTunerCtx->sAtdAgcPara.AgcRfMin  = 0x80;
    psTunerCtx->sAtdAgcPara.AgcIfMin  = 0xD5;
    psTunerCtx->sAtdAgcPara.AgcDrSel  = 0x0B;

    pEUCtx->m_SAW_BW  = C_ASCOT3HELENE_SAW_BW;
    // pEUCtx->m_Ana_Top = C_RE215_TOP_SET_ANA;
    //Tuner frequency range
    pEUCtx->m_s4FreqBoundUpper = C_ASCOT3HELENE_FREQ_DBOUND_UPPER;
    pEUCtx->m_s4FreqBoundLower = C_ASCOT3HELENE_FREQ_DBOUND_LOWER;
    pEUCtx->m_s4AnaFreqBoundUpper = C_ASCOT3HELENE_FREQ_DBOUND_UPPER_Ana;
    pEUCtx->m_s4AnaFreqBoundLower = C_ASCOT3HELENE_FREQ_DBOUND_LOWER_Ana;

    pEUCtx->m_Small_Step = C_ASCOT3HELENE_CHANNEL_SCAN_JUMP_SMALL_STEP;
    pEUCtx->m_Middle_Step_Up = C_ASCOT3HELENE_CHANNEL_SCAN_JUMP_MIDDLE_STEP_UP;
    pEUCtx->m_Middle_Step_Down = C_ASCOT3HELENE_CHANNEL_SCAN_JUMP_MIDDLE_STEP_DOWN;

    pEUCtx->m_aucPara[2] = (UINT8)C_UINT8_ASCOT3HELENE_AGC_IF_SLP;
    pEUCtx->m_aucPara[3] = (UINT8)C_UINT8_ASCOT3HELENE_AGC_IF_INT;
    pEUCtx->m_aucPara[4] = (UINT8)C_UINT8_ASCOT3HELENE_AGC_IF_MAX;
    pEUCtx->m_aucPara[5] = (UINT8)C_UINT8_ASCOT3HELENE_AGC_IF_MIN;

    pUSCtx->m_aucPara_SLD[2] = (UINT8)C_UINT8_ASCOT3HELENE_AGC_IF_SLP;
    pUSCtx->m_aucPara_SLD[3] = (UINT8)C_UINT8_ASCOT3HELENE_AGC_IF_INT;
    pUSCtx->m_aucPara_SLD[4] = (UINT8)C_UINT8_ASCOT3HELENE_AGC_IF_MAX;
    pUSCtx->m_aucPara_SLD[5] = (UINT8)C_UINT8_ASCOT3HELENE_AGC_IF_MIN;

    pEUCtx->m_aucPara[6] = C_ASCOT3HELENE_AGC_COEF;
    // pEUCtx->m_aucPara[7] = C_ASCOT3HELENE_DEMOD_INPUT_POWER;

    pEUCtx->m_aucPara[ 8] = (UINT8)C_UINT8_ASCOT3HELENE_MAX_IF_GAIN_SET;
    pEUCtx->m_aucPara[ 9] = (UINT8)C_UINT8_ASCOT3HELENE_MIN_IF_GAIN_SET;
    pEUCtx->m_aucPara[10] = (UINT8)C_UINT8_ASCOT3HELENE_MAX_RF_GAIN_SET;
    pEUCtx->m_aucPara[11] = (UINT8)C_UINT8_ASCOT3HELENE_MIN_RF_GAIN_SET;
    pEUCtx->m_aucPara[12] = (UINT8)C_UINT8_ASCOT3HELENE_IF_GAIN_RANGE;
    pEUCtx->m_aucPara[13] = (UINT8)C_UINT8_ASCOT3HELENE_RF_GAIN_RANGE;
    pEUCtx->m_aucPara[14] = C_ASCOT3HELENE_POWER_CALIBRATE;
    //IF MAX setting!
    pEUCtx->m_aucPara[21] = 0x7F; //xiangfu 20100526

    pUSCtx->u2LO_Step = 100; //*10HZ
    // init EQ script
    pASCOT3HELENEEqNormal[MOD_NTSC_M - MOD_ANA_TYPE_BEGIN]  = EQASCOT3HELENEM;
    pASCOT3HELENEEqNormal[MOD_PAL_BG - MOD_ANA_TYPE_BEGIN]  = EQASCOT3HELENEBG;
    pASCOT3HELENEEqNormal[MOD_PAL_DK - MOD_ANA_TYPE_BEGIN]  = EQASCOT3HELENEDK;
    pASCOT3HELENEEqNormal[MOD_PAL_I - MOD_ANA_TYPE_BEGIN]  = EQASCOT3HELENEI;
    pASCOT3HELENEEqNormal[MOD_SECAM_L - MOD_ANA_TYPE_BEGIN]  = EQASCOT3HELENEL;
    pASCOT3HELENEEqNormal[MOD_SECAM_L1 - MOD_ANA_TYPE_BEGIN]  = EQASCOT3HELENEL1;

    pASCOT3HELENEEqWeak[MOD_NTSC_M - MOD_ANA_TYPE_BEGIN]  = EQASCOT3HELENEMWeak;
    pASCOT3HELENEEqWeak[MOD_PAL_BG - MOD_ANA_TYPE_BEGIN]  = EQASCOT3HELENEBGWeak;
    pASCOT3HELENEEqWeak[MOD_PAL_DK - MOD_ANA_TYPE_BEGIN]  = EQASCOT3HELENEDKWeak;
    pASCOT3HELENEEqWeak[MOD_PAL_I - MOD_ANA_TYPE_BEGIN]  = EQASCOT3HELENEIWeak;
    pASCOT3HELENEEqWeak[MOD_SECAM_L - MOD_ANA_TYPE_BEGIN]  = EQASCOT3HELENELWeak;
    pASCOT3HELENEEqWeak[MOD_SECAM_L1 - MOD_ANA_TYPE_BEGIN]  = EQASCOT3HELENEL1Weak;

#endif // SONYRF_DUMMY_DEBUG

    {
        sonyrf_tuner_ctx * pSonyRfTunerCtx = &g_sonyrf_tuner_ctx[tunerID];
        pSonyRfTunerCtx->fgCable = 0;
        if(initTimes == 1)
        {
            pSonyRfTunerCtx->tunerInited = FALSE; // Set Non-Inited
        }  
    }
}

//----------------------------------------------------------------------------- 
/**  ASCOT3HELENE_TunerInitHw
 *  Tuner HW initialzation for ASCOT3HELENE.
 *  I2C access should not be done in ASCOT3HELENE_TunerInit but in this API.
 *  @param  pTCtx       Pointer of pi tuner driver context.
 *  @retval void
 */
//-----------------------------------------------------------------------------
static VOID ASCOT3HELENE_TunerInitHw(UINT8 tunerID, ITUNER_CTX_T *pTCtx)
{
    sonyrf_tuner_ctx * pSonyRfTunerCtx = &g_sonyrf_tuner_ctx[tunerID];

        sonyrf_result_t sonyrf_result = SONYRF_RESULT_OK;
        sonyrf_i2c_t * pI2c = &pSonyRfTunerCtx->i2c;
        sonyrf_ascot3_t * pAscot = &pSonyRfTunerCtx->tuner.ascot3;
        sonyrf_helene_t * pHelene = &pSonyRfTunerCtx->tuner.helene;

    if(pSonyRfTunerCtx->tunerInited){
        mcDBG_MSG_L2(("ASCOT3HELENETuner[%d] : Tuner HW has already inited.\n", tunerID));
        return;
    }

    mcDBG_MSG_L2(("ASCOT3HELENETuner[%d] : %s \n", tunerID, __FUNCTION__));

    switch((int) g_sonyproduct_typeinfo.modelVariationID){
    case 0x00: // AEP/PA-T2S2-1Tuner
        if(g_sonyproduct_typeinfo.eBoardVer == BV_SCT){
            pSonyRfTunerCtx->tunerIcType = TUNERIC_ASCOT3;
        }else{
            pSonyRfTunerCtx->tunerIcType = TUNERIC_HELENE;
        }

        // SCT setting
        if(g_sonyproduct_typeinfo.eProjectID == 0x01){ // Salvia
            g_param_table_SCT[SONYRF_ASCOT3_ATV_MN_EIAJ].IF_OUT_SEL = PORT2;
            g_param_table_SCT[SONYRF_ASCOT3_ATV_MN_SAP].IF_OUT_SEL = PORT2;
            g_param_table_SCT[SONYRF_ASCOT3_ATV_MN_A2].IF_OUT_SEL = PORT2;
            g_param_table_SCT[SONYRF_ASCOT3_ATV_BG].IF_OUT_SEL = PORT2;
            g_param_table_SCT[SONYRF_ASCOT3_ATV_I].IF_OUT_SEL = PORT2;
            g_param_table_SCT[SONYRF_ASCOT3_ATV_DK].IF_OUT_SEL = PORT2;
            g_param_table_SCT[SONYRF_ASCOT3_ATV_L].IF_OUT_SEL = PORT2;
            g_param_table_SCT[SONYRF_ASCOT3_ATV_L_DASH].IF_OUT_SEL = PORT2;
            g_param_table_SCT[SONYRF_ASCOT3_ATV_MN_EIAJ].AGC_SEL = PORT2;
            g_param_table_SCT[SONYRF_ASCOT3_ATV_MN_SAP].AGC_SEL = PORT2;
            g_param_table_SCT[SONYRF_ASCOT3_ATV_MN_A2].AGC_SEL = PORT2;
            g_param_table_SCT[SONYRF_ASCOT3_ATV_BG].AGC_SEL = PORT2;
            g_param_table_SCT[SONYRF_ASCOT3_ATV_I].AGC_SEL = PORT2;
            g_param_table_SCT[SONYRF_ASCOT3_ATV_DK].AGC_SEL = PORT2;
            g_param_table_SCT[SONYRF_ASCOT3_ATV_L].AGC_SEL = PORT2;
            g_param_table_SCT[SONYRF_ASCOT3_ATV_L_DASH].AGC_SEL = PORT2;
        }
        break;
    case 0x01: // AEP/PA-T2S2-2Tuner
        if(g_sonyproduct_typeinfo.eBoardVer == BV_SCT){
            pSonyRfTunerCtx->tunerIcType = TUNERIC_ASCOT3;
        }else{
            pSonyRfTunerCtx->tunerIcType = TUNERIC_HELENE;
        }
        break;
    case 0x02: // PA/PH-T2-1Tuner
        pSonyRfTunerCtx->tunerIcType = TUNERIC_ASCOT3;
        // SCT setting
        if(g_sonyproduct_typeinfo.eProjectID == 0x01){ // Salvia
            g_param_table_SCT[SONYRF_ASCOT3_ATV_MN_EIAJ].IF_OUT_SEL = PORT2;
            g_param_table_SCT[SONYRF_ASCOT3_ATV_MN_SAP].IF_OUT_SEL = PORT2;
            g_param_table_SCT[SONYRF_ASCOT3_ATV_MN_A2].IF_OUT_SEL = PORT2;
            g_param_table_SCT[SONYRF_ASCOT3_ATV_BG].IF_OUT_SEL = PORT2;
            g_param_table_SCT[SONYRF_ASCOT3_ATV_I].IF_OUT_SEL = PORT2;
            g_param_table_SCT[SONYRF_ASCOT3_ATV_DK].IF_OUT_SEL = PORT2;
            g_param_table_SCT[SONYRF_ASCOT3_ATV_L].IF_OUT_SEL = PORT2;
            g_param_table_SCT[SONYRF_ASCOT3_ATV_L_DASH].IF_OUT_SEL = PORT2;
            g_param_table_SCT[SONYRF_ASCOT3_ATV_MN_EIAJ].AGC_SEL = PORT2;
            g_param_table_SCT[SONYRF_ASCOT3_ATV_MN_SAP].AGC_SEL = PORT2;
            g_param_table_SCT[SONYRF_ASCOT3_ATV_MN_A2].AGC_SEL = PORT2;
            g_param_table_SCT[SONYRF_ASCOT3_ATV_BG].AGC_SEL = PORT2;
            g_param_table_SCT[SONYRF_ASCOT3_ATV_I].AGC_SEL = PORT2;
            g_param_table_SCT[SONYRF_ASCOT3_ATV_DK].AGC_SEL = PORT2;
            g_param_table_SCT[SONYRF_ASCOT3_ATV_L].AGC_SEL = PORT2;
            g_param_table_SCT[SONYRF_ASCOT3_ATV_L_DASH].AGC_SEL = PORT2;
        }
            break;
    case 0x03: // TW-1Tuner
        pSonyRfTunerCtx->tunerIcType = TUNERIC_ASCOT3;
        // Muffin demod is used for DVB-T. For SCT, IF/AGC port should be changed.
        // SCT setting
        if(g_sonyproduct_typeinfo.eProjectID == 0x01){ // Salvia
            g_param_table_SCT[SONYRF_ASCOT3_ATV_MN_EIAJ].IF_OUT_SEL = PORT2;
            g_param_table_SCT[SONYRF_ASCOT3_ATV_MN_SAP].IF_OUT_SEL = PORT2;
            g_param_table_SCT[SONYRF_ASCOT3_ATV_MN_A2].IF_OUT_SEL = PORT2;
            g_param_table_SCT[SONYRF_ASCOT3_ATV_BG].IF_OUT_SEL = PORT2;
            g_param_table_SCT[SONYRF_ASCOT3_ATV_I].IF_OUT_SEL = PORT2;
            g_param_table_SCT[SONYRF_ASCOT3_ATV_DK].IF_OUT_SEL = PORT2;
            g_param_table_SCT[SONYRF_ASCOT3_ATV_L].IF_OUT_SEL = PORT2;
            g_param_table_SCT[SONYRF_ASCOT3_ATV_L_DASH].IF_OUT_SEL = PORT2;
            g_param_table_SCT[SONYRF_ASCOT3_ATV_MN_EIAJ].AGC_SEL = PORT2;
            g_param_table_SCT[SONYRF_ASCOT3_ATV_MN_SAP].AGC_SEL = PORT2;
            g_param_table_SCT[SONYRF_ASCOT3_ATV_MN_A2].AGC_SEL = PORT2;
            g_param_table_SCT[SONYRF_ASCOT3_ATV_BG].AGC_SEL = PORT2;
            g_param_table_SCT[SONYRF_ASCOT3_ATV_I].AGC_SEL = PORT2;
            g_param_table_SCT[SONYRF_ASCOT3_ATV_DK].AGC_SEL = PORT2;
            g_param_table_SCT[SONYRF_ASCOT3_ATV_L].AGC_SEL = PORT2;
            g_param_table_SCT[SONYRF_ASCOT3_ATV_L_DASH].AGC_SEL = PORT2;
        }else{ // Sakura
            g_param_table_SCT[SONYRF_ASCOT3_DTV_DVBT_5].IF_OUT_SEL = PORT1;
            g_param_table_SCT[SONYRF_ASCOT3_DTV_DVBT_6].IF_OUT_SEL = PORT1;
            g_param_table_SCT[SONYRF_ASCOT3_DTV_DVBT_7].IF_OUT_SEL = PORT1;
            g_param_table_SCT[SONYRF_ASCOT3_DTV_DVBT_8].IF_OUT_SEL = PORT1;
            g_param_table_SCT[SONYRF_ASCOT3_DTV_DVBT_5].AGC_SEL = PORT1;
            g_param_table_SCT[SONYRF_ASCOT3_DTV_DVBT_6].AGC_SEL = PORT1;
            g_param_table_SCT[SONYRF_ASCOT3_DTV_DVBT_7].AGC_SEL = PORT1;
            g_param_table_SCT[SONYRF_ASCOT3_DTV_DVBT_8].AGC_SEL = PORT1;
        }
        break;
    case 0x04: // CH/HK-1Tuner
        pSonyRfTunerCtx->tunerIcType = TUNERIC_ASCOT3;
        // Muffin demod is used for DVB-C. For SCT, IF/AGC port should be changed.
        // SCT setting
        if(g_sonyproduct_typeinfo.eProjectID == 0x01){ // Salvia
            g_param_table_SCT[SONYRF_ASCOT3_ATV_MN_EIAJ].IF_OUT_SEL = PORT2;
            g_param_table_SCT[SONYRF_ASCOT3_ATV_MN_SAP].IF_OUT_SEL = PORT2;
            g_param_table_SCT[SONYRF_ASCOT3_ATV_MN_A2].IF_OUT_SEL = PORT2;
            g_param_table_SCT[SONYRF_ASCOT3_ATV_BG].IF_OUT_SEL = PORT2;
            g_param_table_SCT[SONYRF_ASCOT3_ATV_I].IF_OUT_SEL = PORT2;
            g_param_table_SCT[SONYRF_ASCOT3_ATV_DK].IF_OUT_SEL = PORT2;
            g_param_table_SCT[SONYRF_ASCOT3_ATV_L].IF_OUT_SEL = PORT2;
            g_param_table_SCT[SONYRF_ASCOT3_ATV_L_DASH].IF_OUT_SEL = PORT2;
            g_param_table_SCT[SONYRF_ASCOT3_ATV_MN_EIAJ].AGC_SEL = PORT2;
            g_param_table_SCT[SONYRF_ASCOT3_ATV_MN_SAP].AGC_SEL = PORT2;
            g_param_table_SCT[SONYRF_ASCOT3_ATV_MN_A2].AGC_SEL = PORT2;
            g_param_table_SCT[SONYRF_ASCOT3_ATV_BG].AGC_SEL = PORT2;
            g_param_table_SCT[SONYRF_ASCOT3_ATV_I].AGC_SEL = PORT2;
            g_param_table_SCT[SONYRF_ASCOT3_ATV_DK].AGC_SEL = PORT2;
            g_param_table_SCT[SONYRF_ASCOT3_ATV_L].AGC_SEL = PORT2;
            g_param_table_SCT[SONYRF_ASCOT3_ATV_L_DASH].AGC_SEL = PORT2;
        }else{ // Sakura
            g_param_table_SCT[SONYRF_ASCOT3_DTV_DVBC_6].IF_OUT_SEL = PORT1;
            g_param_table_SCT[SONYRF_ASCOT3_DTV_DVBC_8].IF_OUT_SEL = PORT1;
            g_param_table_SCT[SONYRF_ASCOT3_DTV_DVBC_6].AGC_SEL = PORT1;
            g_param_table_SCT[SONYRF_ASCOT3_DTV_DVBC_8].AGC_SEL = PORT1;
        }
        break;
    case 0x05: // UC/MX-1Tuner
        pSonyRfTunerCtx->tunerIcType = TUNERIC_ASCOT3;
        // SCT setting
        if(g_sonyproduct_typeinfo.eProjectID == 0x01){ // Salvia
            // ATV
            g_param_table_SCT[SONYRF_ASCOT3_ATV_MN_EIAJ].IF_OUT_SEL = PORT2;
            g_param_table_SCT[SONYRF_ASCOT3_ATV_MN_SAP].IF_OUT_SEL = PORT2;
            g_param_table_SCT[SONYRF_ASCOT3_ATV_MN_A2].IF_OUT_SEL = PORT2;
            g_param_table_SCT[SONYRF_ASCOT3_ATV_BG].IF_OUT_SEL = PORT2;
            g_param_table_SCT[SONYRF_ASCOT3_ATV_I].IF_OUT_SEL = PORT2;
            g_param_table_SCT[SONYRF_ASCOT3_ATV_DK].IF_OUT_SEL = PORT2;
            g_param_table_SCT[SONYRF_ASCOT3_ATV_L].IF_OUT_SEL = PORT2;
            g_param_table_SCT[SONYRF_ASCOT3_ATV_L_DASH].IF_OUT_SEL = PORT2;
			
            g_param_table_SCT[SONYRF_ASCOT3_ATV_MN_EIAJ].AGC_SEL = PORT2;
            g_param_table_SCT[SONYRF_ASCOT3_ATV_MN_SAP].AGC_SEL = PORT2;
            g_param_table_SCT[SONYRF_ASCOT3_ATV_MN_A2].AGC_SEL = PORT2;
            g_param_table_SCT[SONYRF_ASCOT3_ATV_BG].AGC_SEL = PORT2;
            g_param_table_SCT[SONYRF_ASCOT3_ATV_I].AGC_SEL = PORT2;
            g_param_table_SCT[SONYRF_ASCOT3_ATV_DK].AGC_SEL = PORT2;
            g_param_table_SCT[SONYRF_ASCOT3_ATV_L].AGC_SEL = PORT2;
            g_param_table_SCT[SONYRF_ASCOT3_ATV_L_DASH].AGC_SEL = PORT2;
            //DTV
            g_param_table_SCT[SONYRF_ASCOT3_DTV_8VSB].IF_OUT_SEL = PORT2;
            g_param_table_SCT[SONYRF_ASCOT3_DTV_QAM].IF_OUT_SEL = PORT2;
            g_param_table_SCT[SONYRF_ASCOT3_DTV_8VSB].AGC_SEL = PORT2;
            g_param_table_SCT[SONYRF_ASCOT3_DTV_QAM].AGC_SEL = PORT2;
        }
        break;
    case 0x06: // LA-1Tuner
        pSonyRfTunerCtx->tunerIcType = TUNERIC_ASCOT3;
        // SCT setting
        if(g_sonyproduct_typeinfo.eProjectID == 0x01){ // Salvia
	        //ATV
	        g_param_table_SCT[SONYRF_ASCOT3_ATV_MN_EIAJ].IF_OUT_SEL = PORT2;
	        g_param_table_SCT[SONYRF_ASCOT3_ATV_MN_SAP].IF_OUT_SEL = PORT2;
	        g_param_table_SCT[SONYRF_ASCOT3_ATV_MN_A2].IF_OUT_SEL = PORT2;
	        g_param_table_SCT[SONYRF_ASCOT3_ATV_BG].IF_OUT_SEL = PORT2;
	        g_param_table_SCT[SONYRF_ASCOT3_ATV_I].IF_OUT_SEL = PORT2;
	        g_param_table_SCT[SONYRF_ASCOT3_ATV_DK].IF_OUT_SEL = PORT2;
	        g_param_table_SCT[SONYRF_ASCOT3_ATV_L].IF_OUT_SEL = PORT2;
	        g_param_table_SCT[SONYRF_ASCOT3_ATV_L_DASH].IF_OUT_SEL = PORT2;

	        g_param_table_SCT[SONYRF_ASCOT3_ATV_MN_EIAJ].AGC_SEL = PORT2;
	        g_param_table_SCT[SONYRF_ASCOT3_ATV_MN_SAP].AGC_SEL = PORT2;
	        g_param_table_SCT[SONYRF_ASCOT3_ATV_MN_A2].AGC_SEL = PORT2;
	        g_param_table_SCT[SONYRF_ASCOT3_ATV_BG].AGC_SEL = PORT2;
	        g_param_table_SCT[SONYRF_ASCOT3_ATV_I].AGC_SEL = PORT2;
	        g_param_table_SCT[SONYRF_ASCOT3_ATV_DK].AGC_SEL = PORT2;
	        g_param_table_SCT[SONYRF_ASCOT3_ATV_L].AGC_SEL = PORT2;
	        g_param_table_SCT[SONYRF_ASCOT3_ATV_L_DASH].AGC_SEL = PORT2;
        }
        break;
    case 0x07: // LA-2Tuner
        pSonyRfTunerCtx->tunerIcType = TUNERIC_ASCOT3;
        break;
    case 0x0A: // Only for Salvia AEP-STD model
        pSonyRfTunerCtx->tunerIcType = TUNERIC_ASCOT3;
        // SCT setting
        if(g_sonyproduct_typeinfo.eProjectID == 0x01){ // Salvia
            g_param_table_SCT[SONYRF_ASCOT3_ATV_MN_EIAJ].IF_OUT_SEL = PORT2;
            g_param_table_SCT[SONYRF_ASCOT3_ATV_MN_SAP].IF_OUT_SEL = PORT2;
            g_param_table_SCT[SONYRF_ASCOT3_ATV_MN_A2].IF_OUT_SEL = PORT2;
            g_param_table_SCT[SONYRF_ASCOT3_ATV_BG].IF_OUT_SEL = PORT2;
            g_param_table_SCT[SONYRF_ASCOT3_ATV_I].IF_OUT_SEL = PORT2;
            g_param_table_SCT[SONYRF_ASCOT3_ATV_DK].IF_OUT_SEL = PORT2;
            g_param_table_SCT[SONYRF_ASCOT3_ATV_L].IF_OUT_SEL = PORT2;
            g_param_table_SCT[SONYRF_ASCOT3_ATV_L_DASH].IF_OUT_SEL = PORT2;
            g_param_table_SCT[SONYRF_ASCOT3_ATV_MN_EIAJ].AGC_SEL = PORT2;
            g_param_table_SCT[SONYRF_ASCOT3_ATV_MN_SAP].AGC_SEL = PORT2;
            g_param_table_SCT[SONYRF_ASCOT3_ATV_MN_A2].AGC_SEL = PORT2;
            g_param_table_SCT[SONYRF_ASCOT3_ATV_BG].AGC_SEL = PORT2;
            g_param_table_SCT[SONYRF_ASCOT3_ATV_I].AGC_SEL = PORT2;
            g_param_table_SCT[SONYRF_ASCOT3_ATV_DK].AGC_SEL = PORT2;
            g_param_table_SCT[SONYRF_ASCOT3_ATV_L].AGC_SEL = PORT2;
            g_param_table_SCT[SONYRF_ASCOT3_ATV_L_DASH].AGC_SEL = PORT2;
        }
        break;
    default:
        // Error!?
        mcDBG_MSG_L2(("ASCOT3HELENETuner[%d] : Fatal Error!! Invalid Model Variation ID!! (%d)\n", tunerID, g_sonyproduct_typeinfo.modelVariationID));
        pSonyRfTunerCtx->tunerIcType = TUNERIC_ASCOT3;
        break;
    }

        // Tuner I2C Connection
    if(g_sonyproduct_typeinfo.eProjectID == 0x01){
        // For Salvia, I2C_D is used
        #ifdef CC_I2C_GATEWAY_SUPPORT
        if(g_sonyproduct_typeinfo.modelVariationID == 0x0a) //PAA model have no external Demod, So No GW
        {
            sonyrf_i2c_MTK_CreateI2c(pI2c, SONYRF_I2C_MTK_BUSID_D);
        }
        else
        {
            sonyrf_i2c_MTK_CreateI2cGw(pI2c, SONY_DEMOD_IIC_ADDRESS_0, 0x09,SONYRF_I2C_MTK_BUSID_D);
        }
        #else
        sonyrf_i2c_MTK_CreateI2c(pI2c, SONYRF_I2C_MTK_BUSID_D);
        #endif
    }else{
        // Single, Dual main tuner, use I2C_A and direct access.
        // Dual sub tuner, use I2C_D. Use Repeater for SCT, and direct access for CVT or later.
        if(tunerID == 1){
            #ifdef CC_I2C_GATEWAY_SUPPORT
            sonyrf_i2c_MTK_CreateI2cGw(pI2c, SONY_DEMOD_IIC_ADDRESS_1, 0x09,SONYRF_I2C_MTK_BUSID_D);
            #else
            sonyrf_i2c_MTK_CreateI2c(pI2c, SONYRF_I2C_MTK_BUSID_D);
            #endif
        }else{
            #ifdef CC_I2C_GATEWAY_SUPPORT
            sonyrf_i2c_MTK_CreateI2cGw(pI2c, SONY_DEMOD_IIC_ADDRESS_0, 0x09,SONYRF_I2C_MTK_BUSID_A);
            #else
            sonyrf_i2c_MTK_CreateI2c(pI2c, SONYRF_I2C_MTK_BUSID_A);
            #endif
            // Note : I2C Repeater will be controlled by demod side.
        }
    }

    // Tuner Driver Create and Initialize
    // For SCT, use sonyrf_ascot3.c/sonyrf_helene.c API directly
    // For CVT or later, use APMD optimized layer
    if(g_sonyproduct_typeinfo.eBoardVer == BV_SCT){
        if(pSonyRfTunerCtx->tunerIcType != TUNERIC_ASCOT3){
            mcDBG_MSG_L2(("ASCOT3HELENETuner[%d] : Fatal Error!! SCT but not ASCOT3 case is not exist.\n", tunerID));
            pSonyRfTunerCtx->tunerIcType = TUNERIC_ASCOT3;
        }

        mcDBG_MSG_L2(("ASCOT3HELENETuner[%d] : SCT ASCOT3 16MHz\n", tunerID));

        sonyrf_ascot3_Create(pAscot, SONYRF_ASCOT3_XTAL_16000KHz,
            0xC0, pI2c, SONYRF_ASCOT3_CONFIG_OVERLOAD_STANDARD | SONYRF_ASCOT3_CONFIG_IFAGCSEL_A1D2);
        pAscot->pParamTable = g_param_table_SCT; // For IF/AGC Port Control

        sonyrf_result = sonyrf_ascot3_Initialize(pAscot);
        if(sonyrf_result != SONYRF_RESULT_OK){
            mcDBG_MSG_L2(("ASCOT3HELENETuner[%d] : sonyrf_ascot3_Initialize failed.\n", tunerID));
        }
    }else{
        // CVT or later
        if(pSonyRfTunerCtx->tunerIcType == TUNERIC_HELENE){
            mcDBG_MSG_L2(("ASCOT3HELENETuner[%d] : CVT(or later) HELENE 24MHz\n", tunerID));

            sonyrf_apmd_helene_module_Create(pHelene, (tunerID == 1) ? 0xC2 : 0xC0, pI2c, (uint8_t)g_sonyproduct_typeinfo.modelVariationID, (uint8_t)g_sonyproduct_typeinfo.eProjectID);

            sonyrf_result = sonyrf_apmd_helene_module_Initialize(pHelene);
            if(sonyrf_result != SONYRF_RESULT_OK){
                mcDBG_MSG_L2(("ASCOT3HELENETuner[%d] : sonyrf_apmd_helene_module_Initialize failed.\n", tunerID));
            }
        }else{
            mcDBG_MSG_L2(("ASCOT3HELENETuner[%d] : CVT(or later) ASCOT3/ASCOT3I 24MHz\n", tunerID));

            sonyrf_apmd_ascot3_module_Create(pAscot, (tunerID == 1) ? 0xC2 : 0xC0, pI2c, (uint8_t)g_sonyproduct_typeinfo.modelVariationID, (uint8_t)g_sonyproduct_typeinfo.eProjectID);

            sonyrf_result = sonyrf_apmd_ascot3_module_Initialize(pAscot);
            if(sonyrf_result != SONYRF_RESULT_OK){
                mcDBG_MSG_L2(("ASCOT3HELENETuner[%d] : sonyrf_apmd_ascot3_module_Create failed.\n", tunerID));
            }
        }
    }

    // LNA ON is automatically done in sonyrf_apmd_xxx_module_Initialize

    pSonyRfTunerCtx->tunerInited = TRUE;
}

//----------------------------------------------------------------------------- 
/**  ASCOT3HELENE_TunerGetStatus
 *  Set Tuner PLL staus for ASCOT3HELENE
 *  @param  pTCtx       Pointer of pi tuner driver context.
 *  @retval TRUE        PLL lock
 *  @retval FALSE       PLL unlock
 */
//-----------------------------------------------------------------------------
static BOOL ASCOT3HELENE_TunerGetStatus(UINT8 tunerID, ITUNER_CTX_T *pTCtx)
{
    sonyrf_tuner_ctx * pSonyRfTunerCtx = &g_sonyrf_tuner_ctx[tunerID];

    sonyrf_result_t sonyrf_result = SONYRF_RESULT_OK;
    sonyrf_i2c_t *pI2c = &pSonyRfTunerCtx->i2c;

    UINT8 data = 0;
    mcDBG_MSG_L2(("ASCOT3HELENETuner[%d] : %s \n", tunerID, __FUNCTION__));

    /* Get DLD */
    sonyrf_result = pI2c->ReadRegister(pI2c, 0xC0, 0x1D, &data, 1);
    if(sonyrf_result != SONYRF_RESULT_OK){
        return FALSE;
    }

    if(data & 0x02){
        return TRUE;
    }else{
        return FALSE;
    }
}

//-----------------------------------------------------------------------------
/** ASCOT3HELENE_TunerSetFreq
 *  Set Tuner PLL for ASCOT3HELENE to tune RF frequency.
 *  @param  pTCtx       Pointer of pi tuner driver context.
 *  @param  param    Pointer of structure of parameters
 *  @retval  0 success
                    1  fail   out of range
                   -1  fail  I2C error
 */
//-----------------------------------------------------------------------------
static INT16 ASCOT3HELENE_TunerSetFreq(UINT8 tunerID, ITUNER_CTX_T *pTCtx, PARAM_SETFREQ_T* param)
{
    sonyrf_tuner_ctx * pSonyRfTunerCtx = &g_sonyrf_tuner_ctx[tunerID];
    sonyrf_ascot3_tv_system_t system = SONYRF_ASCOT3_TV_SYSTEM_UNKNOWN;

    UINT32 Freq = param->Freq; // transfer to video freq(KHz)
    UINT8 Mode = param->Modulation;
    UINT8 bandWidth = pTCtx->specific_member.eu_ctx.m_SAW_BW;

    UINT8 antSw = pSonyRfTunerCtx->fgCable ? 0 : 1; // fgCable is flag to select for analog.

    mcDBG_MSG_L4(("ASCOT3HELENETuner[%d] : %s \n", tunerID, __FUNCTION__));
    pTCtx->u4RF_Freq = Freq; // Update current frequency to context

    mcDBG_MSG_L2(("ASCOT3HELENETuner[%d] : Set Freq %d KHz\n", tunerID, Freq));
    if (Freq < (C_ASCOT3HELENE_FREQ_DBOUND_LOWER/1000) ||
        Freq > (C_ASCOT3HELENE_FREQ_DBOUND_UPPER/1000))
    {
        mcDBG_MSG_L2(("ASCOT3HELENETuner[%d] : Out of range for LO!\n", tunerID));
        //return 1;
    }

#ifdef CC_SONYDEMOD_I2CGATEWAY
    mcDBG_MSG_L4(("ASCOT3HELENETuner : Now using gateway I2C\n"));
#else
    mcDBG_MSG_L4(("ASCOT3HELENETuner : Now using normal I2C\n"));
#endif

 if(!pSonyRfTunerCtx->tunerInited){
	 mcDBG_MSG_L2(("ASCOT3HELENETuner[%d] : WARNING : Tuner initialization is not done!\n", tunerID));
	 ASCOT3HELENE_TunerInitHw(tunerID, pTCtx);
 }
 else{
	 mcDBG_MSG_L4(("ASCOT3HELENETuner[%d] : GOOD : Tuner initialization is done @initialization Stage!\n", tunerID));
 }
   

    switch(Mode){
    /* Digital */
    case MOD_DVBT:
        switch(bandWidth){
        case (UINT8)SAW_BW_6M:
            system = SONYRF_ASCOT3_DTV_DVBT_6;
            pTCtx->u2IF_Freq=C_ASCOT3HELENE_IF_FREQUENCY_DVBT_6M;
            break;
        case (UINT8)SAW_BW_7M:
            system = SONYRF_ASCOT3_DTV_DVBT_7;
            pTCtx->u2IF_Freq=C_ASCOT3HELENE_IF_FREQUENCY_DVBT_7M;
            break;
        case (UINT8)SAW_BW_8M:
            system = SONYRF_ASCOT3_DTV_DVBT_8;
            pTCtx->u2IF_Freq=C_ASCOT3HELENE_IF_FREQUENCY_DVBT_8M;
            break;
        }
        antSw = 1;
        break;
    case MOD_DVBT2:
        switch(bandWidth){
        case (UINT8)SAW_BW_6M:
            system = SONYRF_ASCOT3_DTV_DVBT2_6;
            pTCtx->u2IF_Freq=C_ASCOT3HELENE_IF_FREQUENCY_DVBT2_6M;
            break;
        case (UINT8)SAW_BW_7M:
            system = SONYRF_ASCOT3_DTV_DVBT2_7;
            pTCtx->u2IF_Freq=C_ASCOT3HELENE_IF_FREQUENCY_DVBT2_7M;
            break;
        case (UINT8)SAW_BW_8M:
            system = SONYRF_ASCOT3_DTV_DVBT2_8;
            pTCtx->u2IF_Freq=C_ASCOT3HELENE_IF_FREQUENCY_DVBT2_8M;
            break;
        case (UINT8)SAW_BW_1_7M:
            system = SONYRF_ASCOT3_DTV_DVBT2_1_7;
            pTCtx->u2IF_Freq=C_ASCOT3HELENE_IF_FREQUENCY_DVBT2_1_7M;
            break;
        }
        antSw = 1;
        break;
    case MOD_DVBC:
        system = SONYRF_ASCOT3_DTV_DVBC_8;
        pTCtx->u2IF_Freq=C_ASCOT3HELENE_IF_FREQUENCY_DVBC;
        antSw = 0;
        break;
    case MOD_DVBC2:
        switch(bandWidth){
        case (UINT8)SAW_BW_6M:
            system = SONYRF_ASCOT3_DTV_DVBC2_6;
            pTCtx->u2IF_Freq=C_ASCOT3HELENE_IF_FREQUENCY_DVBC2_6M;
            break;
        case (UINT8)SAW_BW_8M:
            system = SONYRF_ASCOT3_DTV_DVBC2_8;
            pTCtx->u2IF_Freq=C_ASCOT3HELENE_IF_FREQUENCY_DVBC2_8M;
            break;
        }
        antSw = 0;
        break;
    case MOD_ISDBT:
        switch(bandWidth){
        case (UINT8)SAW_BW_6M:
            system = SONYRF_ASCOT3_DTV_ISDBT_6;
            pTCtx->u2IF_Freq=C_ASCOT3HELENE_IF_FREQUENCY_ISDBT_6M;
            break;
        case (UINT8)SAW_BW_7M:
            system = SONYRF_ASCOT3_DTV_ISDBT_7;
            pTCtx->u2IF_Freq=C_ASCOT3HELENE_IF_FREQUENCY_ISDBT_7M;
            break;
        case (UINT8)SAW_BW_8M:
            system = SONYRF_ASCOT3_DTV_ISDBT_8;
            pTCtx->u2IF_Freq=C_ASCOT3HELENE_IF_FREQUENCY_ISDBT_8M;
            break;
        }
        antSw = 1;
        break;
    case MOD_DTMB:
        system = SONYRF_ASCOT3_DTV_DTMB;
        pTCtx->u2IF_Freq=C_ASCOT3HELENE_IF_FREQUENCY_DTMB;
        antSw = 1;
        break;
    case MOD_ATSC_8VSB:
        system = SONYRF_ASCOT3_DTV_8VSB;
        pTCtx->u2IF_Freq=C_ASCOT3HELENE_IF_FREQUENCY_ATSC;
        antSw = 1;
        break;
    case MOD_J83B_64QAM:
    case MOD_J83B_256QAM:
    case MOD_J83B_AUTO_QAM:
        system = SONYRF_ASCOT3_DTV_QAM;
        pTCtx->u2IF_Freq=C_ASCOT3HELENE_IF_FREQUENCY_QAM;
        antSw = 0;
        break;

    /* Analog */
    case MOD_PAL_BG:
        system = SONYRF_ASCOT3_ATV_BG;
        pTCtx->u2IF_Freq_A=C_ASCOT3HELENE_IF_FREQUENCY_PAL_BG;
        break;
    case MOD_PAL_I:
        system = SONYRF_ASCOT3_ATV_I;
        pTCtx->u2IF_Freq_A=C_ASCOT3HELENE_IF_FREQUENCY_PAL_I;
        break;
    case MOD_PAL_DK:
        system = SONYRF_ASCOT3_ATV_DK;
        pTCtx->u2IF_Freq_A=C_ASCOT3HELENE_IF_FREQUENCY_PAL_DK;
        break;
    case MOD_SECAM_L:
        system = SONYRF_ASCOT3_ATV_L;
        pTCtx->u2IF_Freq_A=C_ASCOT3HELENE_IF_FREQUENCY_SEMCAL_L;
        break;
    case MOD_SECAM_L1:
        system = SONYRF_ASCOT3_ATV_L_DASH;
        pTCtx->u2IF_Freq_A=C_ASCOT3HELENE_IF_FREQUENCY_SEMCAL_L1;
        break;
    case MOD_NTSC_M:
        system = SONYRF_ASCOT3_ATV_MN_SAP;
        pTCtx->u2IF_Freq_A=C_ASCOT3HELENE_IF_FREQUENCY_NTSC;
        break;
    default:
       // mcDBG_MSG_ERR(("Unknown MOD_XXX type! \n"));
       // return 1;
       break;
    }


    {
        sonyrf_result_t sonyrf_result = SONYRF_RESULT_OK;
        sonyrf_ascot3_t * pAscot = &pSonyRfTunerCtx->tuner.ascot3;
        sonyrf_helene_t * pHelene = &pSonyRfTunerCtx->tuner.helene;

        // Ant SW Setting (Only for ASCOT3, non-CH/HK model)
        if((pSonyRfTunerCtx->tunerIcType == TUNERIC_ASCOT3) && (g_sonyproduct_typeinfo.modelVariationID != 0x04)){
            sonyrf_result = sonyrf_apmd_ascot3_module_AntSw(pAscot, antSw);
            if(sonyrf_result != SONYRF_RESULT_OK){
                mcDBG_MSG_L2(("ASCOT3HELENETuner[%d] : sonyrf_apmd_ascot3_module_AntSw failed.\n", tunerID));
                return -1;
            }
        }

        if(g_sonyproduct_typeinfo.eBoardVer == BV_SCT){
            // ASCOT3 Only
            sonyrf_result = sonyrf_ascot3_Tune(pAscot, Freq, system);
            if(sonyrf_result != SONYRF_RESULT_OK){
                mcDBG_MSG_L2(("ASCOT3HELENETuner[%d] : sonyrf_ascot3_Tune failed.\n", tunerID));
                return -1;
            }
        }else{
            // CVT or later
            if(pSonyRfTunerCtx->tunerIcType == TUNERIC_HELENE){
                // NOTE: sonyrf_ascot3_tv_system_t and sonyrf_helene_tv_system_t are same value definition for terrestrial/cable
                sonyrf_result = sonyrf_apmd_helene_module_terr_Tune(pHelene, Freq, (sonyrf_helene_tv_system_t)system);
                if(sonyrf_result != SONYRF_RESULT_OK){
                    mcDBG_MSG_L2(("ASCOT3HELENETuner[%d] : sonyrf_apmd_helene_module_terr_Tune failed.\n", tunerID));
                    return -1;
                }
            }else{
                sonyrf_result = sonyrf_apmd_ascot3_module_Tune(pAscot, Freq, system);
                if(sonyrf_result != SONYRF_RESULT_OK){
                    mcDBG_MSG_L2(("ASCOT3HELENETuner[%d] : sonyrf_apmd_ascot3_module_terr_Tune failed.\n", tunerID));
                    return -1;
                }

            }
        }
    }

    // Register dump after tuning. (For Debug)
#if 0
    {
        UINT8 data[256];

        pTuner->pI2c->ReadRegister(pTuner->pI2c, pTuner->i2cAddress, 0x00, data, 256);

        mcDBG_MSG_L2(("Register Dump Result. (Tuner)\n"));
        mcDBG_MSG_L2(("     00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\n"));
        {
            int i = 0;
            int j = 0;
            char dumpline[53];
            char* ptr = dumpline;
            const char hexchar[] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};

            dumpline[52] = '\0';
            for(i=0; i<16; i++){
                ptr = dumpline;
                for(j=0; j<16; j++){
                    if(j == 0){
                        *ptr++ = hexchar[(i*16) >> 4];
                        *ptr++ = hexchar[(i*16) & 0x0F];
                        *ptr++ = ' ';
                        *ptr++ = '|';
                    }
                    *ptr++ = ' ';
                    *ptr++ = hexchar[data[(i*16)+j] >> 4];
                    *ptr++ = hexchar[data[(i*16)+j] & 0x0F];
                }
                mcDBG_MSG_L2(("%s\n", dumpline));
            }
        }
        mcDBG_MSG_L2(("\n"));
    }
#endif

    return 0;
}

//-----------------------------------------------------------------------------
/** ASCOT3HELENE_TunerGetVer
 *  Get Tuner type version
 *  @param  pTCtx       Pointer of pi tuner driver context.
 *  @retval tuner model name
 */
//-----------------------------------------------------------------------------
static CHAR *ASCOT3HELENE_TunerGetVer(UINT8 tunerID)
{
    return (C_SONYRF_ASCOT3HELENE_STR);
}

//-----------------------------------------------------------------------------
/** ASCOT3HELENE_SetSawBw
 *  Set Tuner for Cable or Air input mode
 *  @param  pTunerCtx       Pointer of pi tuner driver context.
 *  @param  sawbw           Bandwidth (SAW_BW_XM)
 *  @retval void
 */
//-----------------------------------------------------------------------------
static void  ASCOT3HELENE_SetSawBw(UINT8 tunerID, ITUNER_CTX_T * pTunerCtx, UINT8 sawbw)
{
    pTunerCtx->specific_member.eu_ctx.m_SAW_BW = sawbw;

    mcDBG_MSG_L2(("ASCOT3HELENETuner[%d]: Saw BW = %d\n", tunerID, sawbw));
}

//----------------------------------------------------------------------------- 
/** ASCOT3HELENE_SetInMode
 *  Set Tuner for Cable or Air input mode
 *  @param  pTunerCtx       Pointer of pi tuner driver context.
 *  @param  bMode           Mode (0: Air, 1: Cable)
 *  @retval void
 */
//-----------------------------------------------------------------------------
static void ASCOT3HELENE_SetInMode(UINT8 tunerID, ITUNER_CTX_T * pTunerCtx, BOOL bMode)
{
    sonyrf_tuner_ctx * pSonyRfTunerCtx = &g_sonyrf_tuner_ctx[tunerID];

    pSonyRfTunerCtx->fgCable = bMode;
    mcDBG_MSG_L2(("ASCOT3HELENETuner[%d]: In Mode = %d (0: Air, 1:Cable)\n", tunerID, bMode));  
}

//----------------------------------------------------------------------------- 
/** ASCOT3HELENE_SetInMode
 *  Set Tuner for Cable or Air input mode
 *  @param  pTunerCtx       Pointer of pi tuner driver context.
 *  @param  bMode           Mode (0: Air, 1: Cable)
 *  @retval void
 */
//-----------------------------------------------------------------------------
void ASCOT3HELENE_SetTerrLNA(UINT8 tunerID, ITUNER_CTX_T * pTunerCtx, UINT8 setVal)
{
    sonyrf_tuner_ctx * pSonyRfTunerCtx = &g_sonyrf_tuner_ctx[tunerID];
    sonyrf_result_t sonyrf_result = SONYRF_RESULT_OK;
    sonyrf_ascot3_t * pAscot = &pSonyRfTunerCtx->tuner.ascot3;
    sonyrf_helene_t * pHelene = &pSonyRfTunerCtx->tuner.helene;

    if(tunerID != 0){
        mcDBG_MSG_L2(("ASCOT3HELENETuner[%d] : Sub tuner cannot control terr external LNA.\n", tunerID));
        return;
    }

    if(g_sonyproduct_typeinfo.eBoardVer == BV_SCT){
        if(g_sonyproduct_typeinfo.eProjectID == 0x01){ //Salvia
            if(g_sonyproduct_typeinfo.modelVariationID == 0x04) //Salvia CHD model
            {
                sonyrf_result = sonyrf_apmd_ascot3_module_ExtLna(pAscot, setVal);
                if(sonyrf_result != SONYRF_RESULT_OK){
                    mcDBG_MSG_L2(("ASCOT3HELENETuner[%d] : sonyrf_apmd_ascot3_module_ExtLna failed.\n", tunerID));
                    return;
                }
            }
        }
    }else{
        if(pSonyRfTunerCtx->tunerIcType == TUNERIC_HELENE){
            sonyrf_result = sonyrf_apmd_helene_module_TerrExtLna(pHelene, setVal);
            if(sonyrf_result != SONYRF_RESULT_OK){
                mcDBG_MSG_L2(("ASCOT3HELENETuner[%d] : sonyrf_apmd_helene_module_ExtLna failed.\n", tunerID));
                return;
            }
        }else{
            sonyrf_result = sonyrf_apmd_ascot3_module_ExtLna(pAscot, setVal);
            if(sonyrf_result != SONYRF_RESULT_OK){
                mcDBG_MSG_L2(("ASCOT3HELENETuner[%d] : sonyrf_apmd_ascot3_module_ExtLna failed.\n", tunerID));
                return;
            }
        }
    }

    mcDBG_MSG_L2(("ASCOT3HELENETuner[%d]: Terr LNA = %d\n", tunerID, setVal));

    return;
}

//-----------------------------------------------------------------------------
/** ASCOT3HELENE_GetTunerChipRevision
 *  Get TunerChipRevision
 *  @retval UINT8 GetTunerChipRevision value
 */
//-----------------------------------------------------------------------------
static UINT8 ASCOT3HELENE_GetChipRev(UINT8 tunerID)
{
    sonyrf_tuner_ctx * pSonyRfTunerCtx = &g_sonyrf_tuner_ctx[tunerID];
    sonyrf_result_t sonyrf_result = SONYRF_RESULT_OK;
    sonyrf_ascot3_t * pAscot = &pSonyRfTunerCtx->tuner.ascot3;
    sonyrf_helene_t * pHelene = &pSonyRfTunerCtx->tuner.helene;
    
    uint8_t tunerChipRev = 0;
    uint8_t tunerType = 0;
    
    tunerType = (int)g_sonyproduct_typeinfo.modelVariationID;
    if((tunerType == 0x00) ||(tunerType == 0x01)){
        sonyrf_result = pHelene->pI2c->ReadRegister(pHelene->pI2c, pHelene->i2cAddress, 0x7F, &tunerChipRev, 1);
        if(sonyrf_result != SONYRF_RESULT_OK){ SONYRF_TRACE_RETURN(SONYRF_RESULT_ERROR_I2C); }
        mcDBG_MSG_L4(("TUNER_CHIP_REVISION is 0x%x\n", tunerChipRev));
    }
    else{
        sonyrf_result = pAscot->pI2c->ReadRegister(pAscot->pI2c, pAscot->i2cAddress, 0x7F, &tunerChipRev, 1);
        if(sonyrf_result != SONYRF_RESULT_OK){ SONYRF_TRACE_RETURN(SONYRF_RESULT_ERROR_I2C); }
        mcDBG_MSG_L4(("TUNER_CHIP_REVISION is 0x%x\n", tunerChipRev));
    }
    return (UINT8)tunerChipRev;
    
}

//-----------------------------------------------------------------------------
/** ASCOT3HELENE_GetRSSI
 *  Get RSSI value in dB x 100
 *  @retval INT32 RSSI value
 */
//-----------------------------------------------------------------------------
static INT32 ASCOT3HELENE_GetRSSI(UINT8 tunerID)
{
    sonyrf_tuner_ctx * pSonyRfTunerCtx = &g_sonyrf_tuner_ctx[tunerID];
    sonyrf_result_t sonyrf_result = SONYRF_RESULT_OK;
    sonyrf_ascot3_t * pAscot = &pSonyRfTunerCtx->tuner.ascot3;
    sonyrf_helene_t * pHelene = &pSonyRfTunerCtx->tuner.helene;

    int32_t rssi_x100 = 0;

    if(g_sonyproduct_typeinfo.eBoardVer == BV_SCT){
        sonyrf_result = sonyrf_ascot3_ReadRssi(pAscot, &rssi_x100);
        if(sonyrf_result != SONYRF_RESULT_OK){
            mcDBG_MSG_L2(("ASCOT3HELENETuner[%d] : sonyrf_ascot3_ReadRssi failed.\n", tunerID));
            rssi_x100 = 0;
        }
        switch(pAscot->tvSystem){
        case SONYRF_ASCOT3_DTV_8VSB:
        case SONYRF_ASCOT3_DTV_QAM:
            rssi_x100 += 0;
            break;
        case SONYRF_ASCOT3_DTV_ISDBT_6:
        case SONYRF_ASCOT3_DTV_ISDBT_7:
        case SONYRF_ASCOT3_DTV_ISDBT_8:
            rssi_x100 += -450;
            break;
        case SONYRF_ASCOT3_DTV_DVBT_5:
        case SONYRF_ASCOT3_DTV_DVBT_6:
        case SONYRF_ASCOT3_DTV_DVBT_7:
        case SONYRF_ASCOT3_DTV_DVBT_8:
            rssi_x100 += -400;
            break;
        case SONYRF_ASCOT3_DTV_DVBT2_1_7:
        case SONYRF_ASCOT3_DTV_DVBT2_5:
        case SONYRF_ASCOT3_DTV_DVBT2_6:
        case SONYRF_ASCOT3_DTV_DVBT2_7:
        case SONYRF_ASCOT3_DTV_DVBT2_8:
            rssi_x100 += -400;
            break;
        case SONYRF_ASCOT3_DTV_DVBC_6:
        case SONYRF_ASCOT3_DTV_DVBC_8:
            rssi_x100 += -550;
            break;
        case SONYRF_ASCOT3_DTV_DVBC2_6:
        case SONYRF_ASCOT3_DTV_DVBC2_8:
            rssi_x100 += -400;
            break;
        case SONYRF_ASCOT3_DTV_DTMB:
            rssi_x100 += -900;
            break;
        default:
            rssi_x100 += 0;
            break;
        }
    }else{
        if(pSonyRfTunerCtx->tunerIcType == TUNERIC_HELENE){
            sonyrf_result = sonyrf_apmd_helene_module_RSSI(pHelene, &rssi_x100);
            if(sonyrf_result != SONYRF_RESULT_OK){
                mcDBG_MSG_L2(("ASCOT3HELENETuner[%d] : sonyrf_apmd_helene_module_RSSI failed.\n", tunerID));
                rssi_x100 = 0;
            }
        }else{
            sonyrf_result = sonyrf_apmd_ascot3_module_RSSI(pAscot, &rssi_x100);
            if(sonyrf_result != SONYRF_RESULT_OK){
                mcDBG_MSG_L2(("ASCOT3HELENETuner[%d] : sonyrf_apmd_ascot3_module_RSSI failed.\n", tunerID));
                rssi_x100 = 0;
            }
        }
    }

    mcDBG_MSG_L2(("ASCOT3HELENETuner[%d] : RSSI (dB x 100) = %d\n", tunerID, rssi_x100));

    //MTK add offset
    {
        if(rssi_x100 >= -9100)
        {
            rssi_x100 = rssi_x100 + 400;
        }
        else
        {
            rssi_x100 = -9500;
        }
        mcDBG_MSG_L2(("ASCOT3HELENETuner[%d] : Sony Demod RSSI (dB x 100) = %d\n", tunerID, rssi_x100));
    }
    return (INT32)rssi_x100;
}

//----------------------------------------------------------------------------- 
/** ASCOT3HELENE_SetRefOut
 *  Set Tuner REFOUT
 *  @param  pTunerCtx       Pointer of pi tuner driver context.
 *  @param  setVal          0 : Disable, 1 : Enable
 *  @retval void
 */
//-----------------------------------------------------------------------------
void ASCOT3HELENE_SetRefOut(UINT8 tunerID, ITUNER_CTX_T * pTunerCtx, UINT8 setVal)
{
    sonyrf_tuner_ctx * pSonyRfTunerCtx = &g_sonyrf_tuner_ctx[tunerID];
    sonyrf_result_t sonyrf_result = SONYRF_RESULT_OK;
    sonyrf_i2c_t * pI2c = &pSonyRfTunerCtx->i2c;
    uint8_t i2cAddress = 0;
    
    if(g_sonyproduct_typeinfo.eBoardVer == BV_SCT)
    {
        mcDBG_MSG_L2(("ASCOT3HELENETuner[%d] : Only CVT Tuner Need Support --Set REFOUT Function--\n", tunerID));
        return;
    }
    else
    {
        if(pSonyRfTunerCtx->tunerIcType == TUNERIC_HELENE){
            i2cAddress = pSonyRfTunerCtx->tuner.helene.i2cAddress;
        }else{
            i2cAddress = pSonyRfTunerCtx->tuner.ascot3.i2cAddress;
        }

        sonyrf_result = pI2c->WriteOneRegister(pI2c, i2cAddress, 0x84, setVal ? 0x80 : 0x00);
        if(sonyrf_result != SONYRF_RESULT_OK){
            mcDBG_MSG_L1(("ASCOT3HELENETuner[%d] : REFOUT setting failed!\n", tunerID));
            return;
        }
        mcDBG_MSG_L2(("ASCOT3HELENETuner[%d] : Set REFOUT (%d)\n", tunerID, setVal));
    }

}

#ifndef SONYRF_DUMMY_DEBUG

static void ASCOT3HELENE_SetSSICondition(UINT8 tunerID, ITUNER_CTX_T* pTunerCtx, VOID * pInOutVal)
{
    pTunerCtx->sSSIPara.i2Mod = *((INT16*) pInOutVal);
    pTunerCtx->sSSIPara.i2CR = *((INT16*) pInOutVal+1);
    pTunerCtx->sSSIPara.i2Ssi_RF= *((INT16 *) pInOutVal+2);
    pTunerCtx->sSSIPara.i2Ssi_IF= *((INT16 *) pInOutVal+3);
    pTunerCtx->sSSIPara.i2Ssi_BB= *((INT16 *) pInOutVal+4);
}

//-----------------------------------------------------------------------------
/** ASCOT3HELENE_GetSSIIndex
 *  Get SSI index
 *  @param fgDVBTC: 0 for get DVBT SSI, 1 for get DVBC SSI
 *  @param
 *  @retval UINT32 RSSI index
 */
//-----------------------------------------------------------------------------
static UINT32 ASCOT3HELENE_GetSSIIndex(UINT8 tunerID, UINT8 fgDVBTC,ITUNER_CTX_T* pTunerCtx)
{
    static const INT16 DVBC_Ref[5] =
    {
        -82, -79, -76, -73, -70
    };

    static const INT16 DVBT_Ref[3][5]=
    {
        /*CR 1/2   2/3   3/4   5/6   7/8 */        /*Modulation*/
        { -93, -91, -90, -89, -88 },    /*  QPSK */
        { -87, -85, -84, -83, -82 },    /* 16QAM */
        { -82, -80, -78, -77, -76 }     /* 64QAM */
    };

    INT32 i4Power_ref = 0;
    INT32 i4Power_rec = 0;
    INT32 i4Power_rel = 0;
    UINT32 u4SSI_Indicator = 0;
    UINT8 ucQamModeUnknown = 3;
    UINT8 ucQamMode = 0;
    UINT8 ucCodeRate = 0;
    SPECIFIC_MEMBER_EU_CTX * pCtx= &(pTunerCtx->specific_member.eu_ctx);
    pCtx->m_SigLvScan = 0;
    ucQamMode = pTunerCtx->sSSIPara.i2Mod;
    ucCodeRate = pTunerCtx->sSSIPara.i2CR;

    if(0 == fgDVBTC)
    {
        mcDBG_MSG_L2(("ASCOT3HELENE_GETSSI_DVBT: Mod=%d, Code Rate=%d",ucQamMode,ucCodeRate));
        ucQamModeUnknown = 3;
    }
    else
    {
        mcDBG_MSG_L2(("ASCOT3HELENE_GETSSI_DVBC: Mod=%d",ucQamMode));
        ucQamModeUnknown = 5;
    }
    if(ucQamMode == ucQamModeUnknown)
    {
        mcDBG_MSG_L1(("QAM mode unknown!\n"));
        return u4SSI_Indicator;
    }
    else
    {
        if(0 == fgDVBTC)
        {
            i4Power_ref = DVBT_Ref[ucQamMode][ucCodeRate];
        }
        else
        {
            i4Power_ref = DVBC_Ref[ucQamMode];
        }
    }
    i4Power_rec = (ASCOT3HELENE_GetRSSI(tunerID)-50)/100;  //*100->*1
    i4Power_rel = i4Power_rec - i4Power_ref;

    mcDBG_MSG_L2(("ASCOT3HELENE_GETSSI: Power_rec=%d, Power_ref=%d, Power_rel=%d\n",i4Power_rec,i4Power_ref,i4Power_rel));
    if(i4Power_rel < -15)
    {
        u4SSI_Indicator = 0;
    }
    else if((i4Power_rel >= -15) && (i4Power_rel < 0))
    {
        u4SSI_Indicator = 2*(i4Power_rel+15)/3;
    }
    else if((i4Power_rel >= 0) && (i4Power_rel < 20))
    {
        u4SSI_Indicator = 4*i4Power_rel+10;
    }
    else if((i4Power_rel >= 20) && (i4Power_rel < 35))
    {
        u4SSI_Indicator = 2*(i4Power_rel-20)/3+90;
    }
    else if(i4Power_rel >= 35)
    {
        u4SSI_Indicator = 100;
    }
    pCtx->m_SigLvScan = (UINT8) u4SSI_Indicator;
    return u4SSI_Indicator;
}

#endif // SONYRF_DUMMY_DEBUG

//-----------------------------------------------------------------------------
/** ASCOT3HELENE_TunerOP
 * Set/Get function
 *  @param  pTCtx       Pointer of pi tuner driver context.
 *  @param  eOperation       To Get/Set operation id
 *  @retval  ITUNER_OK    Operation Success
 *  @retval  ITUNER_NOT_OK    Operation fail
 *  @retval  ITUNER_NOT_SUPPORTED Operation not supported in  driver
 */
//-----------------------------------------------------------------------------
static INT16 ASCOT3HELENE_TunerOP(UINT8 tunerID, ITUNER_CTX_T * pTCtx, ITUNEROP_T eOperation, UINT8 SetVal,  VOID * pInOutVal)
{
    switch(eOperation){
    case itSetTunerInit:
        if(SetVal){
            sonyrf_tuner_ctx * pSonyRfTunerCtx = &g_sonyrf_tuner_ctx[tunerID];
            if(!pSonyRfTunerCtx->tunerInited){
                ASCOT3HELENE_TunerInitHw(tunerID, pTCtx); // tunerInited flag will be set TRUE
            }
        }else{
            sonyrf_tuner_ctx * pSonyRfTunerCtx = &g_sonyrf_tuner_ctx[tunerID];
            pSonyRfTunerCtx->tunerInited = FALSE;
            initTimes = 0;
        }
        break;
    case itGetStatus:
        {
            BOOL * Retval=(BOOL *)pInOutVal;
            *Retval = ASCOT3HELENE_TunerGetStatus(tunerID, pTCtx);
        }
        break;
    case itGetVer:
        {
            CHAR ** RetStr=(CHAR **)pInOutVal;
            *RetStr = ASCOT3HELENE_TunerGetVer(tunerID);
        }
        break;
    case itGetChipRev:
        {
            
            UINT8 * Retval = (UINT8*)pInOutVal;
            * Retval= ASCOT3HELENE_GetChipRev(tunerID);
        }
        break;
    case itSetSawBw:
        ASCOT3HELENE_SetSawBw(tunerID, pTCtx, SetVal);
        break;
    case itSetInCableMode: 
        ASCOT3HELENE_SetInMode(tunerID, pTCtx, (BOOL)SetVal);
        break; 
    case itSetLNAUI:
        ASCOT3HELENE_SetTerrLNA(tunerID, pTCtx, SetVal);
        break;
    case itSetRefOut:
        ASCOT3HELENE_SetRefOut(tunerID, pTCtx, SetVal);
        break;

#ifndef SONYRF_DUMMY_DEBUG

    case itGetEqScriptNormal:
        *(UINT8**)pInOutVal = pASCOT3HELENEEqNormal[SetVal];
        break;

    case itGetEqScriptWeak:
        *(UINT8**)pInOutVal = pASCOT3HELENEEqWeak[SetVal];
        break;
    case itGetTunerRelReg:
    {
        UINT8 subSysId = SetVal;
        UINT8 u1PRATar,u1Dagc,u1DagcTarH,u1DagcTarL,u1PRAConf;

        u1PRATar =0x10;
        u1DagcTarH=0x2c;
        u1DagcTarL=0x2c;
        u1PRAConf=0x88;
        u1Dagc=0x08;
        switch(subSysId)
        {
        case MOD_SECAM_L:
        case MOD_SECAM_L1:
            u1PRATar =0x05;
            u1Dagc=0x88;
            u1DagcTarH=0x2a;
            u1DagcTarL=0x2a;
            u1PRAConf=0xBB;
            break;
        case MOD_NTSC_M:
            u1PRATar =0x10;
            u1Dagc=0x08;
            u1DagcTarH=0x2a;
            u1DagcTarL=0x2a;
            break;
        case MOD_PAL_I:
            u1PRATar =0x10;
            u1Dagc=0x08;
            u1DagcTarH=0x2a;
            u1DagcTarL=0x2a;
            break;
        default://PAL

            break;
        }
        //PRA target
        arrAtdRelReg[PRA_TARGET_H_INDEX].u1RegVal=u1PRATar;
        arrAtdRelReg[PRA_TARGET_L_INDEX].u1RegVal=u1PRATar;
        arrAtdRelReg[SLD_DAGC_00_INDEX].u1RegVal=u1Dagc;
        arrAtdRelReg[DAGC_TARGET_LEVEL_H_INDEX].u1RegVal=u1DagcTarH;
        arrAtdRelReg[DAGC_TARGET_LEVEL_L_INDEX].u1RegVal=u1DagcTarL;
        arrAtdRelReg[PRA_LOOP_CONF_INDEX].u1RegVal=u1PRAConf;

        *(ATD_TUNER_REL_REG_T **)pInOutVal=arrAtdRelReg;
        }
        break;

    case itSetSSICond:
        {
            ASCOT3HELENE_SetSSICondition(tunerID, pTCtx, pInOutVal);
        }
        break;
    case itGetSSIIndex:
        {
            UINT32 * Retval = (UINT32*)pInOutVal;
	   if(((int) g_sonyproduct_typeinfo.modelVariationID) == 0x05)
	   	{
		   	*Retval = (ASCOT3HELENE_GetRSSI(tunerID)-50)/100;  //*100->*1
	   	}
	   	else
	   	{
		   	*Retval = ASCOT3HELENE_GetSSIIndex(tunerID, SetVal, pTCtx);
	   	}
        }
        break;
#endif // SONYRF_DUMMY_DEBUG
    case itGetRSSI:
        {
            INT32 * Retval = (INT32*)pInOutVal;
            *Retval = ASCOT3HELENE_GetRSSI(tunerID);
        }
        break;
    case itSetTrapThresold:
        {
            // Not implemented for now
            mcDBG_MSG_L2(("ASCOT3HELENETuner[%d]: Not supported.\n", tunerID));

            /*
            UINT32 threshold = *(UINT32*)pInOutVal;
            if(SetVal == 0){ // Digital
                // Change digital threshold
                g_TrapThresholdFrequencyDigital = threshold;
                mcDBG_MSG_L2(("ASCOT3HELENETuner: Trap threshold (digital) = %d\n", threshold));
            }else{ // Analog
                // Change analog threshold
                g_TrapThresholdFrequencyAnalog = threshold;
                mcDBG_MSG_L2(("ASCOT3HELENETuner: Trap threshold (analog) = %d\n", threshold));
            }
            */
        }
        break;

#ifndef SONYRF_DUMMY_DEBUG
	case itSetRSetting:
	{
		if (u1Rsetting >= 1)    // >=1:have IF attenuator
	        	{
	        	switch(SetVal)
		     {
				case MOD_PAL_BG: 
				case MOD_PAL_DK: 
				case MOD_PAL_I:
				case MOD_NTSC_M:
					*(UINT32 *)pInOutVal=0x05 ;
				break;
				case MOD_SECAM_L:
				case MOD_SECAM_L1:
					*(UINT32 *)pInOutVal=0x05 ;
				break;
				case MOD_ATSC_8VSB:
				case MOD_J83B_64QAM:
				case MOD_J83B_256QAM:
				case MOD_J83B_AUTO_QAM:
					*(UINT32 *)pInOutVal=0x05 ;
				break;
				case MOD_DVBT:
				case MOD_DVBC:
					*(UINT32 *)pInOutVal=0x05 ;
				break;
				
				default:
			*(UINT32 *)pInOutVal=0x00 ;
		          break;
	          }
	        	}
			
	          	else                       // =0:No IF attenuator
	          	{
	          	switch(SetVal)
		      {
				case MOD_PAL_BG: 
				case MOD_PAL_DK: 
				case MOD_PAL_I:
				case MOD_NTSC_M:
					*(UINT32 *)pInOutVal=0x00 ;
				break;
				case MOD_SECAM_L:
				case MOD_SECAM_L1:
					*(UINT32 *)pInOutVal=0x00 ;
				break;
				case MOD_ATSC_8VSB:
				case MOD_J83B_64QAM:
				case MOD_J83B_256QAM:
				case MOD_J83B_AUTO_QAM:
					*(UINT32 *)pInOutVal=0x00 ;
				break;
				case MOD_DVBT:
				case MOD_DVBC:
					*(UINT32 *)pInOutVal=0x00 ;
				break;
				default:
			*(UINT32 *)pInOutVal=0x00 ;
		          break;
	          		}
	          	}
			
		mcDBG_MSG_L1(("Tuner R_setting Value=%d\n", *(UINT32 *)pInOutVal));
	}
	break;
#endif // SONYRF_DUMMY_DEBUG

    default:
        return ITUNER_NOT_SUPPORTED;
    }

    return ITUNER_OK;
}

//-----------------------------------------------------------------------------
// Public APIs
//-----------------------------------------------------------------------------
// Tuner #0
VOID ASCOT3HELENE_TunerInit_0(ITUNER_CTX_T *pTCtx)
{
    ASCOT3HELENE_TunerInit(0, pTCtx);
}
INT16 ASCOT3HELENE_TunerSetFreq_0(ITUNER_CTX_T *pTCtx, PARAM_SETFREQ_T * param)
{
    return ASCOT3HELENE_TunerSetFreq(0, pTCtx, param);
}
INT16 ASCOT3HELENE_TunerOP_0(ITUNER_CTX_T * pTCtx, ITUNEROP_T eOperation, UINT8 SetVal,  VOID * pInOutVal)
{
    return ASCOT3HELENE_TunerOP(0, pTCtx, eOperation, SetVal, pInOutVal);
}

// Tuner #1
VOID ASCOT3HELENE_TunerInit_1(ITUNER_CTX_T *pTCtx)
{
    ASCOT3HELENE_TunerInit(1, pTCtx);
}
INT16 ASCOT3HELENE_TunerSetFreq_1(ITUNER_CTX_T *pTCtx, PARAM_SETFREQ_T * param)
{
    return ASCOT3HELENE_TunerSetFreq(1, pTCtx, param);
}
INT16 ASCOT3HELENE_TunerOP_1(ITUNER_CTX_T * pTCtx, ITUNEROP_T eOperation, UINT8 SetVal,  VOID * pInOutVal)
{
    return ASCOT3HELENE_TunerOP(1, pTCtx, eOperation, SetVal, pInOutVal);
}


//-----------------------------------------------------------------------------
// Satellite APIs
// NOTE: These APIs are called from demod driver DIRECTLY.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/** HELENE_Satellite_SetRefOut
 *  Set REFOUT enable/disable for Satellite.
 *  @param   setVal  0 : Disable, 1 : Enable
 *  @retval  0   success
 */
//-----------------------------------------------------------------------------
INT16 HELENE_Satellite_SetRefOut(UINT8 tunerID, UINT8 setVal)
{
    if(tunerID >= TUNERCTX_NUM){
        tunerID = 0; // Tuner ID check for safe. (This API is "public".)
    }

    ASCOT3HELENE_SetRefOut(tunerID, NULL, setVal);

    return 0;
}

//-----------------------------------------------------------------------------
/** HELENE_Satellite_Tune
 *  Set Tuner PLL for ASCOT3HELENE to tune RF frequency for Satellite.
 *  @param   freq       Frequency to be tuned. (kHz)
 *  @param   tvSystem   0: DVB-S, 1: DVB-S2, 2: ISDB-S
 *  @param   symbolRate Symbol rate (ksps)
 *  @retval  0   success
 *           1   fail  out of range
 *           -1  fail  I2C error
 */
//-----------------------------------------------------------------------------
INT16 HELENE_Satellite_Tune(UINT8 tunerID, UINT32 freq, UINT8 tvSystem, UINT32 symbolRate)
{
    sonyrf_tuner_ctx * pSonyRfTunerCtx;
    sonyrf_result_t sonyrf_result = SONYRF_RESULT_OK;
    sonyrf_helene_t * pHelene;
    sonyrf_helene_tv_system_t tvSystemHelene;

    if(tunerID >= TUNERCTX_NUM){
        tunerID = 0; // Tuner ID check for safe. (This API is "public".)
    }
    pSonyRfTunerCtx = &g_sonyrf_tuner_ctx[tunerID];
    pHelene = &pSonyRfTunerCtx->tuner.helene;

    mcDBG_MSG_L2(("HELENETuner[%d]: Satellite Tune Called.\n", tunerID));
    mcDBG_MSG_L2(("HELENETuner[%d]: Frequency = %d kHz\n", tunerID, freq));

    if(!pSonyRfTunerCtx->tunerInited){
        mcDBG_MSG_L2(("HELENETuner[%d] : WARNING : Tuner initialization is not done!\n", tunerID));
        ASCOT3HELENE_TunerInitHw(tunerID, NULL);
    }

    switch(tvSystem){
    case 0:
        tvSystemHelene = SONYRF_HELENE_STV_DVBS;
        mcDBG_MSG_L2(("HELENETuner[%d]: System = DVB-S\n", tunerID));
        break;
    case 1:
        tvSystemHelene = SONYRF_HELENE_STV_DVBS2;
        mcDBG_MSG_L2(("HELENETuner[%d]: System = DVB-S2\n", tunerID));
        break;
    case 2:
        tvSystemHelene = SONYRF_HELENE_STV_ISDBS;
        mcDBG_MSG_L2(("HELENETuner[%d]: System = ISDB-S\n", tunerID));
        break;
    default:
        mcDBG_MSG_L2(("HELENETuner[%d]: System = Unknown!! (%d)\n", tunerID, tvSystem));
        return 1;
    }

    mcDBG_MSG_L2(("HELENETuner[%d]: Symbol Rate = %d ksps\n", tunerID, symbolRate));

    sonyrf_result = sonyrf_apmd_helene_module_sat_Tune(pHelene, (uint32_t)freq, tvSystemHelene, (uint32_t)symbolRate);
    if(sonyrf_result != SONYRF_RESULT_OK){
        mcDBG_MSG_L2(("HELENETuner[%d]: Satellite Tune Failed!!\n", tunerID));
        return -1;
    }

    return 0;
}

//-----------------------------------------------------------------------------
/** HELENE_Satellite_Sleep
 *  Sleep the tuner.
 *  @retval  0   success
 *           -1  fail  I2C error
 */
//-----------------------------------------------------------------------------
INT16 HELENE_Satellite_Sleep(UINT8 tunerID)
{
    sonyrf_tuner_ctx * pSonyRfTunerCtx;
    sonyrf_result_t sonyrf_result = SONYRF_RESULT_OK;
    sonyrf_helene_t * pHelene;

    if(tunerID >= TUNERCTX_NUM){
        tunerID = 0; // Tuner ID check for safe. (This API is "public".)
    }
    pSonyRfTunerCtx = &g_sonyrf_tuner_ctx[tunerID];
    pHelene = &pSonyRfTunerCtx->tuner.helene;

    mcDBG_MSG_L2(("HELENETuner[%d]: Satellite Sleep Called.\n", tunerID));

    sonyrf_result = sonyrf_apmd_helene_module_Sleep(pHelene);
    if(sonyrf_result != SONYRF_RESULT_OK){
        mcDBG_MSG_L2(("HELENETuner[%d]: Satellite Sleep Failed!!\n", tunerID));
        return -1;
    }

    return 0;
}

//-----------------------------------------------------------------------------
/** HELENE_Satellite_AntSw
 *  Switch Antenna SW. (For Dual Tuner, TBD)
 *  @param   sw  Switch status.
 *  @retval  0   success
 *           -1  fail  I2C error
 */
//-----------------------------------------------------------------------------
INT16 HELENE_Satellite_AntSw(UINT8 tunerID, UINT8 sw)
{
    sonyrf_tuner_ctx * pSonyRfTunerCtx;
    sonyrf_result_t sonyrf_result = SONYRF_RESULT_OK;
    sonyrf_helene_t * pHelene;

    if(tunerID != 1){
        mcDBG_MSG_L2(("HELENETuner[%d] : Main tuner cannot control sat switch.\n", tunerID));
        return -1;
    }

    pSonyRfTunerCtx = &g_sonyrf_tuner_ctx[tunerID];
    pHelene = &pSonyRfTunerCtx->tuner.helene;

    mcDBG_MSG_L2(("HELENETuner[%d]: Satellite AndSW Called.\n", tunerID));
    mcDBG_MSG_L2(("HELENETuner[%d]: SW = %d\n", tunerID, sw));

    sonyrf_result = sonyrf_apmd_helene_module_satAntSw(pHelene, sw);
    if(sonyrf_result != SONYRF_RESULT_OK){
        mcDBG_MSG_L2(("HELENETuner[%d]: Satellite AndSW Failed!!\n", tunerID));
        return -1;
    }

    return 0;
}

/* For driver debug trace --------------------------------------------------- */
#ifdef SONYRF_TRACE_ENABLE
void sonyrf_trace_log_enter(const char* funcname, const char* filename, unsigned int linenum)
{
    mcDBG_MSG_L4(("ASCOT3HELENE_ENTER : %s (%d)\n", funcname, linenum));
}
void sonyrf_trace_log_return(sonyrf_result_t result, const char* filename, unsigned int linenum)
{
    if(result != SONYRF_RESULT_OK){
        mcDBG_MSG_L4(("ASCOT3HELENE_ERROR_RETURN(%d) : (line %d)\n", result, linenum));
    }else{
        mcDBG_MSG_L4(("ASCOT3HELENE_RETURN : (line %d))\n", linenum));
    }
}

void sonyrf_trace_i2c_log_enter(const char* funcname, const char* filename, unsigned int linenum)
{
    sonyrf_trace_log_enter(funcname, filename, linenum);
}
void sonyrf_trace_i2c_log_return(sonyrf_result_t result, const char* filename, unsigned int linenum)
{
    sonyrf_trace_log_return(result, filename, linenum);
}
#endif /* SONYRF_TRACE_ENABLE */
