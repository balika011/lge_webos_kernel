/*------------------------------------------------------------------------------
  Copyright 2012 Sony Corporation

  Last Updated  : 2012/07/30
  File Revision : 1.0.3.0
------------------------------------------------------------------------------*/
#include "sonyrf_ascot2e_re227.h"

/*------------------------------------------------------------------------------
 Definitions of static const table
------------------------------------------------------------------------------*/

#define AUTO         (0xFF) /* For AGC_SEL, IF_OUT_SEL, it means that the value is desided by config flags. */
                            /* For RF_GAIN, it means that RF_GAIN_SEL(SubAddr:0x4E) = 1 */
#define PORT1        (0x00)
#define PORT2        (0x01)

#define OFFSET(ofs)  ((uint8_t)(ofs) & 0x1F)
#define BW_1_7       (0x03)
#define BW_6         (0x00)
#define BW_7         (0x01)
#define BW_8         (0x02)

#define CABLE        (0x00)
#define TERR         (0x01)

extern UINT8 fgTunerNormal_GW; //wenming 2012-7-4:flag for tuner Normal or Gateway access, 0 for Normal, 1 for Gateway
#ifdef CC_SONY_EU_TUNERALL
extern UINT8 u1TunerVersion;
enum
{
    TUNER_VERSION_PE221,
    TUNER_VERSION_EVB,
    TUNER_VERSION_ANFIELD
};
#endif

#ifdef CC_SONYDEMOD_CXD2817
/* Optimized table for SUT-Rx22x */
static const sonyrf_ascot2e_adjust_param_t g_param_table_g1_Rx22x[SONYRF_ASCOT2E_TV_SYSTEM_NUM] = {
    /*
    IF_OUT_SEL    MIX_OLL    IF_BPF_GC          BW_OFFSET        RF_OLDET
      |    AGC_SEL  |   RF_GAIN |    FIF_OFFSET   |          BW     |   IF_BPF_F0
      |      |      |     |     |     |           |           |     |     |       */
    {PORT2, PORT2, 0x02, AUTO, 0x03, OFFSET(0),  OFFSET(0),  BW_6, 0x09, 0x00}, /**< SONYRF_ASCOT2E_TV_SYSTEM_UNKNOWN */
    /* Analog */                                                               
    {PORT2, PORT2, 0x02, AUTO, 0x03, OFFSET(0),  OFFSET(0),  BW_6, 0x09, 0x00}, /**< SONYRF_ASCOT2E_ATV_MN_EIAJ  (System-M (Japan)) */
    {PORT2, PORT2, 0x02, AUTO, 0x03, OFFSET(0),  OFFSET(0),  BW_6, 0x09, 0x00}, /**< SONYRF_ASCOT2E_ATV_MN_SAP   (System-M (US)) */
    {PORT2, PORT2, 0x02, AUTO, 0x03, OFFSET(3),  OFFSET(0),  BW_6, 0x09, 0x00}, /**< SONYRF_ASCOT2E_ATV_MN_A2    (System-M (Korea)) */
    {PORT2, PORT2, 0x02, AUTO, 0x03, OFFSET(11), OFFSET(8),  BW_7, 0x09, 0x00}, /**< SONYRF_ASCOT2E_ATV_BG       (System-B/G) */
    {PORT2, PORT2, 0x02, AUTO, 0x03, OFFSET(2),  OFFSET(0),  BW_8, 0x09, 0x00}, /**< SONYRF_ASCOT2E_ATV_I        (System-I) */
    {PORT2, PORT2, 0x02, AUTO, 0x03, OFFSET(2),  OFFSET(0),  BW_8, 0x09, 0x00}, /**< SONYRF_ASCOT2E_ATV_DK       (System-D/K) */
    {PORT2, PORT2, 0x05, AUTO, 0x01, OFFSET(2),  OFFSET(0),  BW_8, 0x0A, 0x00}, /**< SONYRF_ASCOT2E_ATV_L        (System-L) */
    {PORT2, PORT2, 0x05, AUTO, 0x01, OFFSET(-1), OFFSET(7),  BW_8, 0x0A, 0x00}, /**< SONYRF_ASCOT2E_ATV_L_DASH   (System-L DASH) */
    /* Digital */                                                              
    {PORT2, PORT2, 0x03, AUTO, 0x06, OFFSET(-6), OFFSET(-6), BW_6, 0x0B, 0x00}, /**< SONYRF_ASCOT2E_DTV_8VSB     (ATSC 8VSB) */
    {PORT2, PORT2, 0x03, AUTO, 0x06, OFFSET(-6), OFFSET(-6), BW_6, 0x0B, 0x00}, /**< SONYRF_ASCOT2E_DTV_QAM      (US QAM) */
    {PORT2, PORT2, 0x03, AUTO, 0x06, OFFSET(-3), OFFSET(0),  BW_6, 0x0B, 0x00}, /**< SONYRF_ASCOT2E_DTV_ISDBT_6  (ISDB-T 6MHzBW) */
    {PORT2, PORT2, 0x03, AUTO, 0x06, OFFSET(-7), OFFSET(-5), BW_7, 0x0B, 0x00}, /**< SONYRF_ASCOT2E_DTV_ISDBT_7  (ISDB-T 7MHzBW) */
    {PORT2, PORT2, 0x03, AUTO, 0x06, OFFSET(-5), OFFSET(-3), BW_8, 0x0B, 0x00}, /**< SONYRF_ASCOT2E_DTV_ISDBT_8  (ISDB-T 8MHzBW) */
    {PORT2, PORT2, 0x03, AUTO, 0x06, OFFSET(-2), OFFSET(-4), BW_6, 0x0B, 0x00}, /**< SONYRF_ASCOT2E_DTV_DVBT_5   (DVB-T 5MHzBW) */
    {PORT2, PORT2, 0x03, AUTO, 0x06, OFFSET(-2), OFFSET(-4), BW_6, 0x0B, 0x00}, /**< SONYRF_ASCOT2E_DTV_DVBT_6   (DVB-T 6MHzBW) */
    {PORT2, PORT2, 0x03, AUTO, 0x06, OFFSET(-3), OFFSET(0),  BW_7, 0x0B, 0x00}, /**< SONYRF_ASCOT2E_DTV_DVBT_7   (DVB-T 7MHzBW) */
    {PORT2, PORT2, 0x03, AUTO, 0x06, OFFSET(-3), OFFSET(0),  BW_8, 0x0B, 0x00}, /**< SONYRF_ASCOT2E_DTV_DVBT_8   (DVB-T 8MHzBW) */
    {PORT2, PORT2, 0x03, AUTO, 0x06, OFFSET(-10),OFFSET(-16),BW_1_7,0x0B,0x00}, /**< SONYRF_ASCOT2E_DTV_DVBT2_1_7(DVB-T2 1.7MHzBW) */
    {PORT2, PORT2, 0x03, AUTO, 0x06, OFFSET(0),  OFFSET(0),  BW_6, 0x0B, 0x00}, /**< SONYRF_ASCOT2E_DTV_DVBT2_5  (DVB-T2 5MHzBW) */
    {PORT2, PORT2, 0x03, AUTO, 0x06, OFFSET(0),  OFFSET(0),  BW_6, 0x0B, 0x00}, /**< SONYRF_ASCOT2E_DTV_DVBT2_6  (DVB-T2 6MHzBW) */
    {PORT2, PORT2, 0x03, AUTO, 0x06, OFFSET(-6), OFFSET(-2), BW_7, 0x0B, 0x00}, /**< SONYRF_ASCOT2E_DTV_DVBT2_7  (DVB-T2 7MHzBW) */
    {PORT2, PORT2, 0x03, AUTO, 0x06, OFFSET(-6), OFFSET(-3), BW_8, 0x0B, 0x00}, /**< SONYRF_ASCOT2E_DTV_DVBT2_8  (DVB-T2 8MHzBW) */
    {PORT2, PORT2, 0x02, AUTO, 0x03, OFFSET(-1), OFFSET(3),  BW_8, 0x09, 0x00}, /**< SONYRF_ASCOT2E_DTV_DVBC     (DVB-C) */
    {PORT2, PORT2, 0x03, AUTO, 0x01, OFFSET(-6), OFFSET(-4), BW_6, 0x09, 0x00}, /**< SONYRF_ASCOT2E_DTV_DVBC2_6  (DVB-C2 6MHzBW) */
    {PORT2, PORT2, 0x03, AUTO, 0x01, OFFSET(-2), OFFSET(2),  BW_8, 0x09, 0x00}, /**< SONYRF_ASCOT2E_DTV_DVBC2_8  (DVB-C2 8MHzBW) */
    {PORT2, PORT2, 0x03, AUTO, 0x02, OFFSET(2),  OFFSET(3),  BW_8, 0x0B, 0x00}  /**< SONYRF_ASCOT2E_DTV_DTMB     (DTMB) */
};
#else // CC_SONYDEMOD_CXD2817
/* Optimized table for SUT-Rx22x */
static const sonyrf_ascot2e_adjust_param_t g_param_table_g2_Rx22x[SONYRF_ASCOT2E_TV_SYSTEM_NUM] = {
    /*
    IF_OUT_SEL   MIX_OLL    IF_BPF_GC          BW_OFFSET        RF_OLDET
      |    AGC_SEL  |   RF_GAIN |    FIF_OFFSET   |          BW     |   IF_BPF_F0
      |      |      |     |     |     |           |           |     |     |       */
    {PORT2, PORT2, 0x02, AUTO, 0x03, OFFSET(0),  OFFSET(0),  BW_6, 0x09, 0x00}, /**< SONYRF_ASCOT2E_TV_SYSTEM_UNKNOWN */
    /* Analog */
    {PORT1, PORT1, 0x02, AUTO, 0x03, OFFSET(0),  OFFSET(0),  BW_6, 0x09, 0x00}, /**< SONYRF_ASCOT2E_ATV_MN_EIAJ  (System-M (Japan)) */
    {PORT1, PORT1, 0x02, AUTO, 0x03, OFFSET(0),  OFFSET(0),  BW_6, 0x09, 0x00}, /**< SONYRF_ASCOT2E_ATV_MN_SAP   (System-M (US)) */
    {PORT1, PORT1, 0x02, AUTO, 0x03, OFFSET(3),  OFFSET(0),  BW_6, 0x09, 0x00}, /**< SONYRF_ASCOT2E_ATV_MN_A2    (System-M (Korea)) */
    {PORT1, PORT1, 0x02, AUTO, 0x03, OFFSET(11), OFFSET(8),  BW_7, 0x09, 0x00}, /**< SONYRF_ASCOT2E_ATV_BG       (System-B/G) */
    {PORT1, PORT1, 0x02, AUTO, 0x03, OFFSET(2),  OFFSET(0),  BW_8, 0x09, 0x00}, /**< SONYRF_ASCOT2E_ATV_I        (System-I) */
    {PORT1, PORT1, 0x02, AUTO, 0x03, OFFSET(2),  OFFSET(0),  BW_8, 0x09, 0x00}, /**< SONYRF_ASCOT2E_ATV_DK       (System-D/K) */
    {PORT1, PORT1, 0x05, AUTO, 0x01, OFFSET(2),  OFFSET(0),  BW_8, 0x0A, 0x00}, /**< SONYRF_ASCOT2E_ATV_L        (System-L) */
    {PORT1, PORT1, 0x05, AUTO, 0x01, OFFSET(-1), OFFSET(7),  BW_8, 0x0A, 0x00}, /**< SONYRF_ASCOT2E_ATV_L_DASH   (System-L DASH) */
    /* Digital */
    {PORT2, PORT2, 0x03, AUTO, 0x06, OFFSET(-6), OFFSET(-6), BW_6, 0x0B, 0x00}, /**< SONYRF_ASCOT2E_DTV_8VSB     (ATSC 8VSB) */
    {PORT2, PORT2, 0x03, AUTO, 0x06, OFFSET(-6), OFFSET(-6), BW_6, 0x0B, 0x00}, /**< SONYRF_ASCOT2E_DTV_QAM      (US QAM) */
    {PORT2, PORT2, 0x03, AUTO, 0x06, OFFSET(-9), OFFSET(-7), BW_6, 0x0B, 0x00}, /**< SONYRF_ASCOT2E_DTV_ISDBT_6  (ISDB-T 6MHzBW) */
    {PORT2, PORT2, 0x03, AUTO, 0x06, OFFSET(-7), OFFSET(-5), BW_7, 0x0B, 0x00}, /**< SONYRF_ASCOT2E_DTV_ISDBT_7  (ISDB-T 7MHzBW) */
    {PORT2, PORT2, 0x03, AUTO, 0x06, OFFSET(-5), OFFSET(-3), BW_8, 0x0B, 0x00}, /**< SONYRF_ASCOT2E_DTV_ISDBT_8  (ISDB-T 8MHzBW) */
    {PORT2, PORT2, 0x03, AUTO, 0x06, OFFSET(-8), OFFSET(-6), BW_6, 0x0B, 0x00}, /**< SONYRF_ASCOT2E_DTV_DVBT_5   (DVB-T 5MHzBW) */
    {PORT2, PORT2, 0x03, AUTO, 0x06, OFFSET(-8), OFFSET(-6), BW_6, 0x0B, 0x00}, /**< SONYRF_ASCOT2E_DTV_DVBT_6   (DVB-T 6MHzBW) */
    {PORT2, PORT2, 0x03, AUTO, 0x06, OFFSET(-6), OFFSET(-4), BW_7, 0x0B, 0x00}, /**< SONYRF_ASCOT2E_DTV_DVBT_7   (DVB-T 7MHzBW) */
    {PORT2, PORT2, 0x03, AUTO, 0x06, OFFSET(-4), OFFSET(-2), BW_8, 0x0B, 0x00}, /**< SONYRF_ASCOT2E_DTV_DVBT_8   (DVB-T 8MHzBW) */
    {PORT2, PORT2, 0x03, AUTO, 0x06, OFFSET(-10),OFFSET(-16),BW_1_7,0x0B,0x00}, /**< SONYRF_ASCOT2E_DTV_DVBT2_1_7(DVB-T2 1.7MHzBW) */
    {PORT2, PORT2, 0x03, AUTO, 0x06, OFFSET(-8), OFFSET(-6), BW_6, 0x0B, 0x00}, /**< SONYRF_ASCOT2E_DTV_DVBT2_5  (DVB-T2 5MHzBW) */
    {PORT2, PORT2, 0x03, AUTO, 0x06, OFFSET(-8), OFFSET(-6), BW_6, 0x0B, 0x00}, /**< SONYRF_ASCOT2E_DTV_DVBT2_6  (DVB-T2 6MHzBW) */
    {PORT2, PORT2, 0x03, AUTO, 0x06, OFFSET(-6), OFFSET(-4), BW_7, 0x0B, 0x00}, /**< SONYRF_ASCOT2E_DTV_DVBT2_7  (DVB-T2 7MHzBW) */
    {PORT2, PORT2, 0x03, AUTO, 0x06, OFFSET(-4), OFFSET(-2), BW_8, 0x0B, 0x00}, /**< SONYRF_ASCOT2E_DTV_DVBT2_8  (DVB-T2 8MHzBW) */
    {PORT2, PORT2, 0x02, AUTO, 0x03, OFFSET(-2), OFFSET(-1), BW_8, 0x09, 0x00}, /**< SONYRF_ASCOT2E_DTV_DVBC     (DVB-C) */
    {PORT2, PORT2, 0x03, AUTO, 0x01, OFFSET(-6), OFFSET(-4), BW_6, 0x09, 0x00}, /**< SONYRF_ASCOT2E_DTV_DVBC2_6  (DVB-C2 6MHzBW) */
    {PORT2, PORT2, 0x03, AUTO, 0x01, OFFSET(-2), OFFSET(2),  BW_8, 0x09, 0x00}, /**< SONYRF_ASCOT2E_DTV_DVBC2_8  (DVB-C2 8MHzBW) */
    {PORT2, PORT2, 0x03, AUTO, 0x02, OFFSET(2),  OFFSET(3),  BW_8, 0x0B, 0x00}  /**< SONYRF_ASCOT2E_DTV_DTMB     (DTMB) */
};
#ifdef CC_SONY_EU_TUNERALL //This is to support all SONY eu tuner, including the EVB tuner board
static const sonyrf_ascot2e_adjust_param_t g_param_table_g2_Rx22x_Evb[SONYRF_ASCOT2E_TV_SYSTEM_NUM] = {
    /*
    IF_OUT_SEL   MIX_OLL    IF_BPF_GC          BW_OFFSET        RF_OLDET
      |    AGC_SEL  |   RF_GAIN |    FIF_OFFSET   |          BW     |   IF_BPF_F0
      |      |      |     |     |     |           |           |     |     |       */
    {PORT1, PORT1, 0x02, AUTO, 0x03, OFFSET(0),  OFFSET(0),  BW_6, 0x09, 0x00}, /**< SONYRF_ASCOT2E_TV_SYSTEM_UNKNOWN */
    /* Analog */
    {PORT1, PORT1, 0x02, AUTO, 0x03, OFFSET(0),  OFFSET(0),  BW_6, 0x09, 0x00}, /**< SONYRF_ASCOT2E_ATV_MN_EIAJ  (System-M (Japan)) */
    {PORT1, PORT1, 0x02, AUTO, 0x03, OFFSET(0),  OFFSET(0),  BW_6, 0x09, 0x00}, /**< SONYRF_ASCOT2E_ATV_MN_SAP   (System-M (US)) */
    {PORT1, PORT1, 0x02, AUTO, 0x03, OFFSET(3),  OFFSET(0),  BW_6, 0x09, 0x00}, /**< SONYRF_ASCOT2E_ATV_MN_A2    (System-M (Korea)) */
    {PORT1, PORT1, 0x02, AUTO, 0x03, OFFSET(11), OFFSET(8),  BW_7, 0x09, 0x00}, /**< SONYRF_ASCOT2E_ATV_BG       (System-B/G) */
    {PORT1, PORT1, 0x02, AUTO, 0x03, OFFSET(2),  OFFSET(0),  BW_8, 0x09, 0x00}, /**< SONYRF_ASCOT2E_ATV_I        (System-I) */
    {PORT1, PORT1, 0x02, AUTO, 0x03, OFFSET(2),  OFFSET(0),  BW_8, 0x09, 0x00}, /**< SONYRF_ASCOT2E_ATV_DK       (System-D/K) */
    {PORT1, PORT1, 0x05, AUTO, 0x01, OFFSET(2),  OFFSET(0),  BW_8, 0x0A, 0x00}, /**< SONYRF_ASCOT2E_ATV_L        (System-L) */
    {PORT1, PORT1, 0x05, AUTO, 0x01, OFFSET(-1), OFFSET(7),  BW_8, 0x0A, 0x00}, /**< SONYRF_ASCOT2E_ATV_L_DASH   (System-L DASH) */
    /* Digital */
    {PORT1, PORT1, 0x03, AUTO, 0x06, OFFSET(-6), OFFSET(-6), BW_6, 0x0B, 0x00}, /**< SONYRF_ASCOT2E_DTV_8VSB     (ATSC 8VSB) */
    {PORT1, PORT1, 0x03, AUTO, 0x06, OFFSET(-6), OFFSET(-6), BW_6, 0x0B, 0x00}, /**< SONYRF_ASCOT2E_DTV_QAM      (US QAM) */
    {PORT1, PORT1, 0x03, AUTO, 0x06, OFFSET(-9), OFFSET(-7), BW_6, 0x0B, 0x00}, /**< SONYRF_ASCOT2E_DTV_ISDBT_6  (ISDB-T 6MHzBW) */
    {PORT1, PORT1, 0x03, AUTO, 0x06, OFFSET(-7), OFFSET(-5), BW_7, 0x0B, 0x00}, /**< SONYRF_ASCOT2E_DTV_ISDBT_7  (ISDB-T 7MHzBW) */
    {PORT1, PORT1, 0x03, AUTO, 0x06, OFFSET(-5), OFFSET(-3), BW_8, 0x0B, 0x00}, /**< SONYRF_ASCOT2E_DTV_ISDBT_8  (ISDB-T 8MHzBW) */
    {PORT1, PORT1, 0x03, AUTO, 0x06, OFFSET(-8), OFFSET(-6), BW_6, 0x0B, 0x00}, /**< SONYRF_ASCOT2E_DTV_DVBT_5   (DVB-T 5MHzBW) */
    {PORT1, PORT1, 0x03, AUTO, 0x06, OFFSET(-8), OFFSET(-6), BW_6, 0x0B, 0x00}, /**< SONYRF_ASCOT2E_DTV_DVBT_6   (DVB-T 6MHzBW) */
    {PORT1, PORT1, 0x03, AUTO, 0x06, OFFSET(-6), OFFSET(-4), BW_7, 0x0B, 0x00}, /**< SONYRF_ASCOT2E_DTV_DVBT_7   (DVB-T 7MHzBW) */
    {PORT1, PORT1, 0x03, AUTO, 0x06, OFFSET(-4), OFFSET(-2), BW_8, 0x0B, 0x00}, /**< SONYRF_ASCOT2E_DTV_DVBT_8   (DVB-T 8MHzBW) */
    {PORT1, PORT1, 0x03, AUTO, 0x06, OFFSET(-10),OFFSET(-16),BW_1_7,0x0B,0x00}, /**< SONYRF_ASCOT2E_DTV_DVBT2_1_7(DVB-T2 1.7MHzBW) */
    {PORT1, PORT1, 0x03, AUTO, 0x06, OFFSET(-8), OFFSET(-6), BW_6, 0x0B, 0x00}, /**< SONYRF_ASCOT2E_DTV_DVBT2_5  (DVB-T2 5MHzBW) */
    {PORT1, PORT1, 0x03, AUTO, 0x06, OFFSET(-8), OFFSET(-6), BW_6, 0x0B, 0x00}, /**< SONYRF_ASCOT2E_DTV_DVBT2_6  (DVB-T2 6MHzBW) */
    {PORT1, PORT1, 0x03, AUTO, 0x06, OFFSET(-6), OFFSET(-4), BW_7, 0x0B, 0x00}, /**< SONYRF_ASCOT2E_DTV_DVBT2_7  (DVB-T2 7MHzBW) */
    {PORT1, PORT1, 0x03, AUTO, 0x06, OFFSET(-4), OFFSET(-2), BW_8, 0x0B, 0x00}, /**< SONYRF_ASCOT2E_DTV_DVBT2_8  (DVB-T2 8MHzBW) */
    {PORT1, PORT1, 0x02, AUTO, 0x03, OFFSET(-2), OFFSET(-1), BW_8, 0x09, 0x00}, /**< SONYRF_ASCOT2E_DTV_DVBC     (DVB-C) */
    {PORT1, PORT1, 0x03, AUTO, 0x01, OFFSET(-6), OFFSET(-4), BW_6, 0x09, 0x00}, /**< SONYRF_ASCOT2E_DTV_DVBC2_6  (DVB-C2 6MHzBW) */
    {PORT1, PORT1, 0x03, AUTO, 0x01, OFFSET(-2), OFFSET(2),  BW_8, 0x09, 0x00}, /**< SONYRF_ASCOT2E_DTV_DVBC2_8  (DVB-C2 8MHzBW) */
    {PORT1, PORT1, 0x03, AUTO, 0x02, OFFSET(2),  OFFSET(3),  BW_8, 0x0B, 0x00}  /**< SONYRF_ASCOT2E_DTV_DTMB     (DTMB) */
};

static const sonyrf_ascot2e_adjust_param_t g_param_table_g2_Rx22x_Anfield_x[SONYRF_ASCOT2E_TV_SYSTEM_NUM] = {
    /*
    IF_OUT_SEL   MIX_OLL    IF_BPF_GC          BW_OFFSET        RF_OLDET
      |    AGC_SEL  |   RF_GAIN |    FIF_OFFSET   |          BW     |   IF_BPF_F0
      |      |      |     |     |     |           |           |     |     |       */
    {PORT2, PORT2, 0x02, AUTO, 0x03, OFFSET(0),  OFFSET(0),  BW_6, 0x09, 0x00}, /**< SONYRF_ASCOT2E_TV_SYSTEM_UNKNOWN */
    /* Analog */
    {PORT2, PORT2, 0x02, AUTO, 0x03, OFFSET(0),  OFFSET(0),  BW_6, 0x09, 0x00}, /**< SONYRF_ASCOT2E_ATV_MN_EIAJ  (System-M (Japan)) */
    {PORT2, PORT2, 0x02, AUTO, 0x03, OFFSET(0),  OFFSET(0),  BW_6, 0x09, 0x00}, /**< SONYRF_ASCOT2E_ATV_MN_SAP   (System-M (US)) */
    {PORT2, PORT2, 0x02, AUTO, 0x03, OFFSET(3),  OFFSET(0),  BW_6, 0x09, 0x00}, /**< SONYRF_ASCOT2E_ATV_MN_A2    (System-M (Korea)) */
    {PORT2, PORT2, 0x02, AUTO, 0x03, OFFSET(11), OFFSET(8),  BW_7, 0x09, 0x00}, /**< SONYRF_ASCOT2E_ATV_BG       (System-B/G) */
    {PORT2, PORT2, 0x02, AUTO, 0x03, OFFSET(2),  OFFSET(0),  BW_8, 0x09, 0x00}, /**< SONYRF_ASCOT2E_ATV_I        (System-I) */
    {PORT2, PORT2, 0x02, AUTO, 0x03, OFFSET(2),  OFFSET(0),  BW_8, 0x09, 0x00}, /**< SONYRF_ASCOT2E_ATV_DK       (System-D/K) */
    {PORT2, PORT2, 0x05, AUTO, 0x01, OFFSET(2),  OFFSET(0),  BW_8, 0x0A, 0x00}, /**< SONYRF_ASCOT2E_ATV_L        (System-L) */
    {PORT2, PORT2, 0x05, AUTO, 0x01, OFFSET(-1), OFFSET(7),  BW_8, 0x0A, 0x00}, /**< SONYRF_ASCOT2E_ATV_L_DASH   (System-L DASH) */
    /* Digital */
    {PORT2, PORT2, 0x03, AUTO, 0x06, OFFSET(-6), OFFSET(-6), BW_6, 0x0B, 0x00}, /**< SONYRF_ASCOT2E_DTV_8VSB     (ATSC 8VSB) */
    {PORT2, PORT2, 0x03, AUTO, 0x06, OFFSET(-6), OFFSET(-6), BW_6, 0x0B, 0x00}, /**< SONYRF_ASCOT2E_DTV_QAM      (US QAM) */
    {PORT2, PORT2, 0x03, AUTO, 0x06, OFFSET(-9), OFFSET(-7), BW_6, 0x0B, 0x00}, /**< SONYRF_ASCOT2E_DTV_ISDBT_6  (ISDB-T 6MHzBW) */
    {PORT2, PORT2, 0x03, AUTO, 0x06, OFFSET(-7), OFFSET(-5), BW_7, 0x0B, 0x00}, /**< SONYRF_ASCOT2E_DTV_ISDBT_7  (ISDB-T 7MHzBW) */
    {PORT2, PORT2, 0x03, AUTO, 0x06, OFFSET(-5), OFFSET(-3), BW_8, 0x0B, 0x00}, /**< SONYRF_ASCOT2E_DTV_ISDBT_8  (ISDB-T 8MHzBW) */
    {PORT2, PORT2, 0x03, AUTO, 0x06, OFFSET(-8), OFFSET(-6), BW_6, 0x0B, 0x00}, /**< SONYRF_ASCOT2E_DTV_DVBT_5   (DVB-T 5MHzBW) */
    {PORT2, PORT2, 0x03, AUTO, 0x06, OFFSET(-8), OFFSET(-6), BW_6, 0x0B, 0x00}, /**< SONYRF_ASCOT2E_DTV_DVBT_6   (DVB-T 6MHzBW) */
    {PORT2, PORT2, 0x03, AUTO, 0x06, OFFSET(-6), OFFSET(-4), BW_7, 0x0B, 0x00}, /**< SONYRF_ASCOT2E_DTV_DVBT_7   (DVB-T 7MHzBW) */
    {PORT2, PORT2, 0x03, AUTO, 0x06, OFFSET(-4), OFFSET(-2), BW_8, 0x0B, 0x00}, /**< SONYRF_ASCOT2E_DTV_DVBT_8   (DVB-T 8MHzBW) */
    {PORT2, PORT2, 0x03, AUTO, 0x06, OFFSET(-10),OFFSET(-16),BW_1_7,0x0B,0x00}, /**< SONYRF_ASCOT2E_DTV_DVBT2_1_7(DVB-T2 1.7MHzBW) */
    {PORT2, PORT2, 0x03, AUTO, 0x06, OFFSET(-8), OFFSET(-6), BW_6, 0x0B, 0x00}, /**< SONYRF_ASCOT2E_DTV_DVBT2_5  (DVB-T2 5MHzBW) */
    {PORT2, PORT2, 0x03, AUTO, 0x06, OFFSET(-8), OFFSET(-6), BW_6, 0x0B, 0x00}, /**< SONYRF_ASCOT2E_DTV_DVBT2_6  (DVB-T2 6MHzBW) */
    {PORT2, PORT2, 0x03, AUTO, 0x06, OFFSET(-6), OFFSET(-4), BW_7, 0x0B, 0x00}, /**< SONYRF_ASCOT2E_DTV_DVBT2_7  (DVB-T2 7MHzBW) */
    {PORT2, PORT2, 0x03, AUTO, 0x06, OFFSET(-4), OFFSET(-2), BW_8, 0x0B, 0x00}, /**< SONYRF_ASCOT2E_DTV_DVBT2_8  (DVB-T2 8MHzBW) */
    {PORT2, PORT2, 0x02, AUTO, 0x03, OFFSET(-2), OFFSET(-1), BW_8, 0x09, 0x00}, /**< SONYRF_ASCOT2E_DTV_DVBC     (DVB-C) */
    {PORT2, PORT2, 0x03, AUTO, 0x01, OFFSET(-6), OFFSET(-4), BW_6, 0x09, 0x00}, /**< SONYRF_ASCOT2E_DTV_DVBC2_6  (DVB-C2 6MHzBW) */
    {PORT2, PORT2, 0x03, AUTO, 0x01, OFFSET(-2), OFFSET(2),  BW_8, 0x09, 0x00}, /**< SONYRF_ASCOT2E_DTV_DVBC2_8  (DVB-C2 8MHzBW) */
    {PORT2, PORT2, 0x03, AUTO, 0x02, OFFSET(2),  OFFSET(3),  BW_8, 0x0B, 0x00}  /**< SONYRF_ASCOT2E_DTV_DTMB     (DTMB) */
};

#endif
#endif // CC_SONYDEMOD_CXD2817

/* RF trap threshold for RE227 */
uint32_t g_TrapThresholdFrequencyDigital = 9999999; /* Always OFF in default */
uint32_t g_TrapThresholdFrequencyAnalog = 470000;

/*--------------------------------------------------------------------
  Tuner instance creation

  sonyrf_ascot2e_t   *pTuner      Tuner struct instance
  sonyrf_i2c_t       *pI2c        I2c struct instance
--------------------------------------------------------------------*/
sonyrf_result_t sonyrf_ascot2e_re227_Create(sonyrf_ascot2e_t *pTuner, sonyrf_i2c_t *pI2c)
{
    sonyrf_result_t result = SONYRF_RESULT_OK;

    SONYRF_TRACE_ENTER("sonyrf_ascot2e_re227_Create");

    if((!pTuner) || (!pI2c)){
        SONYRF_TRACE_RETURN(SONYRF_RESULT_ERROR_ARG);
    }

    /* 16MHz Xtal, Two IF output (IF/AGC 2: Digital & Analog) */
    result = sonyrf_ascot2e_Create(pTuner, 16, SONYRF_ASCOT2E_ADDRESS, pI2c,
        SONYRF_ASCOT2E_CONFIG_IFAGCSEL_ALL1);
    if(result != SONYRF_RESULT_OK){
        SONYRF_TRACE_RETURN(result);
    }

    /* Set Rx22x optimized table */
#ifdef CC_SONYDEMOD_CXD2817
    pTuner->pParamTable = g_param_table_g1_Rx22x;
#else
#ifdef CC_SONY_EU_TUNERALL
    if(TUNER_VERSION_PE221 == u1TunerVersion)
    {
        pTuner->pParamTable = g_param_table_g2_Rx22x;
    }
    else if(TUNER_VERSION_EVB == u1TunerVersion)
    {
        pTuner->pParamTable = g_param_table_g2_Rx22x_Evb;
    }
    else
    {
        pTuner->pParamTable = g_param_table_g2_Rx22x_Anfield_x;
    }
#else
    pTuner->pParamTable = g_param_table_g2_Rx22x; //for CXD2834/CXD2835/CXD2840
#endif
#endif

    SONYRF_TRACE_RETURN(SONYRF_RESULT_OK);
}

/*--------------------------------------------------------------------
  Tuner initialization

  sonyrf_ascot2e_t   *pTuner      Tuner struct instance
--------------------------------------------------------------------*/
sonyrf_result_t sonyrf_ascot2e_re227_Initialize(sonyrf_ascot2e_t *pTuner)
{
    sonyrf_result_t result = SONYRF_RESULT_OK;

    SONYRF_TRACE_ENTER("sonyrf_ascot2e_re227_Initialize");

    result = sonyrf_ascot2e_Initialize(pTuner);
    if(result != SONYRF_RESULT_OK){
        SONYRF_TRACE_RETURN(result);
    }

    SONYRF_TRACE_RETURN(result);
}

/*--------------------------------------------------------------------
  Tuning

  sonyrf_ascot2e_t          *pTuner      Tuner struct instance
  uint32_t                frequencykHz RF frequency (kHz)
  sonyrf_ascot_tv_system_t  system       Terrestrial broadcasting system ID
--------------------------------------------------------------------*/
sonyrf_result_t sonyrf_ascot2e_re227_Tune(sonyrf_ascot2e_t *pTuner, uint32_t frequencykHz, sonyrf_ascot2e_tv_system_t system)
{
    sonyrf_result_t result = SONYRF_RESULT_OK;
    uint8_t ExtCtrlEnabler;

    SONYRF_TRACE_ENTER("sonyrf_ascot2e_re227_Tune");

    /* NOTE: 0:ON 1:OFF */
    if(SONYRF_ASCOT2E_IS_DTV(system)){
        if(frequencykHz >= g_TrapThresholdFrequencyDigital){
            ExtCtrlEnabler = 0;
        }else{
            ExtCtrlEnabler = 1;
        }
    }else{
        if(frequencykHz >= g_TrapThresholdFrequencyAnalog){
            ExtCtrlEnabler = 0;
        }else{
            ExtCtrlEnabler = 1;
        }
    }

    result = sonyrf_ascot2e_SetRfExtCtrl(pTuner, ExtCtrlEnabler);
    if(result != SONYRF_RESULT_OK){
        SONYRF_TRACE_RETURN(result);
    }

    result = sonyrf_ascot2e_Tune(pTuner, frequencykHz, system);
    if(result != SONYRF_RESULT_OK){
        SONYRF_TRACE_RETURN(result);
    }

#ifdef CC_FE_FAST_CHANNEL_CHANGE
    if(!SONYRF_ASCOT2E_IS_DTV(system))
    {
        if(0 == pTuner->fgAutoSearch)
        {
            SONYRF_SLEEP(20);
        }
        else
        {
            SONYRF_SLEEP(50);
        }
    }
    else
#endif
    {
        SONYRF_SLEEP(50);
    }

    result = sonyrf_ascot2e_TuneEnd(pTuner);

    SONYRF_TRACE_RETURN(result);
}

/*--------------------------------------------------------------------
  Tuner sleep

  sonyrf_ascot2e_t   *pTuner      Tuner struct instance
--------------------------------------------------------------------*/
sonyrf_result_t sonyrf_ascot2e_re227_Sleep(sonyrf_ascot2e_t *pTuner)
{
    sonyrf_result_t result = SONYRF_RESULT_OK;

    SONYRF_TRACE_ENTER("sonyrf_ascot2e_re227_Sleep");

    result = sonyrf_ascot2e_Sleep(pTuner);

    SONYRF_TRACE_RETURN(result);
}

/*--------------------------------------------------------------------
  Control Antenna SW

  sonyrf_ascot2e_t   *pTuner      Tuner struct instance
  uint8_t          sw           Antenna Status (0:cable 1:Terr) 
--------------------------------------------------------------------*/
sonyrf_result_t sonyrf_ascot2e_re227_AntSw(sonyrf_ascot2e_t *pTuner, uint8_t sw)
{
    sonyrf_result_t result = SONYRF_RESULT_OK;

    SONYRF_TRACE_ENTER("sonyrf_ascot2e_re227_AntSw");

    result = sonyrf_ascot2e_SetGPO(pTuner, (uint8_t)1, sw);

    SONYRF_TRACE_RETURN(result);
}

/*--------------------------------------------------------------------
  Shift frequency

  sonyrf_ascot2e_t   *pTuner      Tuner struct instance
  uint32_t         frequencykHz RF frequency (kHz)
--------------------------------------------------------------------*/
sonyrf_result_t sonyrf_ascot2e_re227_Shift(sonyrf_ascot2e_t *pTuner, uint32_t frequencykHz)
{
    sonyrf_result_t result = SONYRF_RESULT_OK;

    SONYRF_TRACE_ENTER("sonyrf_ascot2e_re227_Shift");

    result = sonyrf_ascot2e_ShiftFRF(pTuner, frequencykHz);

    SONYRF_TRACE_RETURN(result);
}

/*--------------------------------------------------------------------
  Get signal strength

  sonyrf_ascot2e_t   *pTuner      Tuner struct instance
--------------------------------------------------------------------*/

sonyrf_result_t sonyrf_ascot2e_re227_RSSI(sonyrf_ascot2e_t *pTuner, int16_t *rssi_x100)
{
    sonyrf_result_t result = SONYRF_RESULT_OK;
    int32_t ifout_x100 = -420; /* This parameter is depend on the demod. */
    int32_t ifgain_x100 = 0;
    int32_t rfgain_x100 = 0;
    int32_t offset_x100 = 0;
	int32_t ifshare_offset_x100 = 400;

    SONYRF_TRACE_ENTER("sonyrf_ascot2e_re227_RSSI");

    result = sonyrf_ascot2e_ReadGain(pTuner, &ifgain_x100, &rfgain_x100, 0);
    if(result != SONYRF_RESULT_OK){
        SONYRF_TRACE_RETURN(result);
    }

#if defined(CC_SONYDEMOD_CXD2817) || defined(CC_SONYDEMOD_CXD2840)
    if(pTuner->tvSystem == SONYRF_ASCOT2E_DTV_DVBC){
        ifout_x100 = -510;
    }
#elif defined(CC_SONYDEMOD_CXD2837) || defined(CC_SONYDEMOD_CXD2842)
    // Nothing to do
#else
    // MTK demod
    if(pTuner->tvSystem == SONYRF_ASCOT2E_DTV_DVBC)
    {
        ifout_x100 = -510;
    }
    else
    {
        ifout_x100 = -610;
    }
#endif

    if(pTuner->frequencykHz <= 192000){
        offset_x100 = 526;
    }else if(pTuner->frequencykHz <= 235000){
        offset_x100 = 448;
    }else if(pTuner->frequencykHz <= 470000){
        offset_x100 = 432;
    }else if(pTuner->frequencykHz <= 600000){
        offset_x100 = 409;
    }else{
        offset_x100 = 406;
    }

    *rssi_x100 = (int16_t)(ifout_x100)  - (int16_t)(ifgain_x100)  - (int16_t)(rfgain_x100) + (int16_t)(offset_x100) + (int16_t)(ifshare_offset_x100);
    
    SONYRF_TRACE_RETURN(SONYRF_RESULT_OK);
}
