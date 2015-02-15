/*------------------------------------------------------------------------------
  Copyright 2014 Sony Corporation

  Last Updated  : 2014/08/08
  File Revision : 1.9.0.2
------------------------------------------------------------------------------*/
#include "sonyrf_apmd_ascot3_module_forBRAVIA.h"

/*------------------------------------------------------------------------------
 Definitions of static const table
------------------------------------------------------------------------------*/

#define AUTO         (0xFF) /* For AGC_SEL, IF_OUT_SEL, it means that the value is desided by config flags. */
                            /* For RF_GAIN, it means that RF_GAIN_SEL(SubAddr:0x4E) = 1 */
#define PORT1        (0x00)
#define PORT2        (0x01)

#define OFFSET(ofs)  ((uint8_t)(ofs) & 0x1F)
#define BW_6         (0x00)
#define BW_7         (0x01)
#define BW_8         (0x02)
#define BW_1_7       (0x03)

#define CABLE        (0x00)
#define TERR         (0x01)

#define LEGACY       (0x00)
#define SINGLECABLE  (0x01)

#define EXTLNA_OFF   (0x00)
#define EXTLNA_ON    (0x01)

#define MODULE_RE241 (0x0A)
#define MODULE_DE241 (0x02)
#define MODULE_RT241 (0x03)
#define MODULE_DC241 (0x04)
#define MODULE_RA241 (0x05)
#define MODULE_DB241 (0x06)
#define MODULE_DB541 (0x07)

static const sonyrf_ascot3_adjust_param_t g_param_table_apmd_ascot3[SONYRF_ASCOT3_TV_SYSTEM_NUM] = {
    /*
    OUTLMT    IF_BPF_GC                                           BW              BW_OFFSET         IF_OUT_SEL
      |  RF_GAIN  |     RFOVLD_DET_LV1    IFOVLD_DET_LV  IF_BPF_F0 |   FIF_OFFSET     |        AGC_SEL |  IS_LOWERLOCAL
      |     |     |    (VL)  (VH)  (U)   (VL)  (VH)  (U)    |      |       |          |          |     |     |          */
    {0x00, AUTO, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, BW_6,  OFFSET(0),  OFFSET(0),  AUTO, AUTO, 0x00}, /**< SONYRF_ASCOT3_TV_SYSTEM_UNKNOWN */
    /* Analog */
    {0x00, AUTO, 0x05, 0x02, 0x05, 0x02, 0x01, 0x01, 0x01, 0x00, BW_6,  OFFSET(0),  OFFSET(1),  AUTO, AUTO, 0x00}, /**< SONYRF_ASCOT3_ATV_MN_EIAJ   (System-M (Japan)) */
    {0x00, AUTO, 0x05, 0x02, 0x05, 0x02, 0x01, 0x01, 0x01, 0x00, BW_6,  OFFSET(0),  OFFSET(1),  AUTO, AUTO, 0x00}, /**< SONYRF_ASCOT3_ATV_MN_SAP    (System-M (US)) */
    {0x00, AUTO, 0x05, 0x02, 0x05, 0x02, 0x01, 0x01, 0x01, 0x00, BW_6,  OFFSET(3),  OFFSET(1),  AUTO, AUTO, 0x00}, /**< SONYRF_ASCOT3_ATV_MN_A2     (System-M (Korea)) */
    {0x00, AUTO, 0x05, 0x02, 0x05, 0x02, 0x01, 0x01, 0x01, 0x00, BW_7,  OFFSET(11), OFFSET(5),  AUTO, AUTO, 0x00}, /**< SONYRF_ASCOT3_ATV_BG        (System-B/G) */
    {0x00, AUTO, 0x05, 0x02, 0x05, 0x02, 0x01, 0x01, 0x01, 0x03, BW_8,  OFFSET(-2), OFFSET(4),  AUTO, AUTO, 0x00}, /**< SONYRF_ASCOT3_ATV_I         (System-I) */
    {0x00, AUTO, 0x05, 0x02, 0x05, 0x02, 0x01, 0x01, 0x01, 0x00, BW_8,  OFFSET(2),  OFFSET(-3), AUTO, AUTO, 0x00}, /**< SONYRF_ASCOT3_ATV_DK        (System-D/K) */
    {0x00, AUTO, 0x05, 0x03, 0x06, 0x00, 0x04, 0x04, 0x04, 0x00, BW_8,  OFFSET(2),  OFFSET(-3), AUTO, AUTO, 0x00}, /**< SONYRF_ASCOT3_ATV_L         (System-L) */
    {0x00, AUTO, 0x05, 0x03, 0x06, 0x00, 0x04, 0x04, 0x04, 0x00, BW_8,  OFFSET(-1), OFFSET(4),  AUTO, AUTO, 0x00}, /**< SONYRF_ASCOT3_ATV_L_DASH    (System-L DASH) */
    /* Digital */
    {0x00, AUTO, 0x09, 0x0A, 0x0A, 0x0A, 0x03, 0x03, 0x03, 0x00, BW_6,  OFFSET(-2), OFFSET(4),  AUTO, AUTO, 0x00}, /**< SONYRF_ASCOT3_DTV_8VSB      (ATSC 8VSB) */
    {0x00, AUTO, 0x09, 0x0A, 0x0A, 0x0A, 0x02, 0x02, 0x02, 0x00, BW_6,  OFFSET(-2), OFFSET(4),  AUTO, AUTO, 0x00}, /**< SONYRF_ASCOT3_DTV_QAM       (US QAM) */
    {0x00, AUTO, 0x09, 0x0A, 0x0A, 0x0A, 0x02, 0x02, 0x02, 0x00, BW_6,  OFFSET(-9), OFFSET(-5), AUTO, AUTO, 0x00}, /**< SONYRF_ASCOT3_DTV_ISDBT_6   (ISDB-T 6MHzBW) */
    {0x00, AUTO, 0x09, 0x0A, 0x0A, 0x0A, 0x02, 0x02, 0x02, 0x00, BW_7,  OFFSET(-7), OFFSET(-6), AUTO, AUTO, 0x00}, /**< SONYRF_ASCOT3_DTV_ISDBT_7   (ISDB-T 7MHzBW) */
    {0x00, AUTO, 0x09, 0x0A, 0x0A, 0x0A, 0x02, 0x02, 0x02, 0x00, BW_8,  OFFSET(-5), OFFSET(-7), AUTO, AUTO, 0x00}, /**< SONYRF_ASCOT3_DTV_ISDBT_8   (ISDB-T 8MHzBW) */
    {0x00, AUTO, 0x09, 0x0A, 0x0A, 0x0A, 0x02, 0x02, 0x02, 0x00, BW_6,  OFFSET(-8), OFFSET(-3), AUTO, AUTO, 0x00}, /**< SONYRF_ASCOT3_DTV_DVBT_5    (DVB-T 5MHzBW) */
    {0x00, AUTO, 0x09, 0x0A, 0x0A, 0x0A, 0x02, 0x02, 0x02, 0x00, BW_6,  OFFSET(-8), OFFSET(-3), AUTO, AUTO, 0x00}, /**< SONYRF_ASCOT3_DTV_DVBT_6    (DVB-T 6MHzBW) */
    {0x00, AUTO, 0x09, 0x0A, 0x0A, 0x0A, 0x02, 0x02, 0x02, 0x00, BW_7,  OFFSET(-6), OFFSET(-5), AUTO, AUTO, 0x00}, /**< SONYRF_ASCOT3_DTV_DVBT_7    (DVB-T 7MHzBW) */
    {0x00, AUTO, 0x09, 0x0A, 0x0A, 0x0A, 0x02, 0x02, 0x02, 0x00, BW_8,  OFFSET(-4), OFFSET(-6), AUTO, AUTO, 0x00}, /**< SONYRF_ASCOT3_DTV_DVBT_8    (DVB-T 8MHzBW) */
    {0x00, AUTO, 0x09, 0x0A, 0x0A, 0x0A, 0x02, 0x02, 0x02, 0x00, BW_1_7,OFFSET(-10),OFFSET(-10),AUTO, AUTO, 0x00}, /**< SONYRF_ASCOT3_DTV_DVBT2_1_7 (DVB-T2 1.7MHzBW) */
    {0x00, AUTO, 0x09, 0x0A, 0x0A, 0x0A, 0x02, 0x02, 0x02, 0x00, BW_6,  OFFSET(-8), OFFSET(-3), AUTO, AUTO, 0x00}, /**< SONYRF_ASCOT3_DTV_DVBT2_5   (DVB-T2 5MHzBW) */
    {0x00, AUTO, 0x09, 0x0A, 0x0A, 0x0A, 0x02, 0x02, 0x02, 0x00, BW_6,  OFFSET(-8), OFFSET(-3), AUTO, AUTO, 0x00}, /**< SONYRF_ASCOT3_DTV_DVBT2_6   (DVB-T2 6MHzBW) */
    {0x00, AUTO, 0x09, 0x0A, 0x0A, 0x0A, 0x02, 0x02, 0x02, 0x00, BW_7,  OFFSET(-6), OFFSET(-5), AUTO, AUTO, 0x00}, /**< SONYRF_ASCOT3_DTV_DVBT2_7   (DVB-T2 7MHzBW) */
    {0x00, AUTO, 0x09, 0x0A, 0x0A, 0x0A, 0x02, 0x02, 0x02, 0x00, BW_8,  OFFSET(-4), OFFSET(-6), AUTO, AUTO, 0x00}, /**< SONYRF_ASCOT3_DTV_DVBT2_8   (DVB-T2 8MHzBW) */
    {0x00, AUTO, 0x05, 0x05, 0x05, 0x05, 0x01, 0x01, 0x01, 0x00, BW_6,  OFFSET(-6), OFFSET(-4), AUTO, AUTO, 0x00}, /**< SONYRF_ASCOT3_DTV_DVBC_6    (DVB-C 6MHzBW) */
    {0x00, AUTO, 0x05, 0x05, 0x05, 0x05, 0x01, 0x01, 0x01, 0x00, BW_8,  OFFSET(-2), OFFSET(-3), AUTO, AUTO, 0x00}, /**< SONYRF_ASCOT3_DTV_DVBC_8    (DVB-C 8MHzBW) */
    {0x00, AUTO, 0x03, 0x08, 0x08, 0x08, 0x02, 0x02, 0x02, 0x00, BW_6,  OFFSET(-6), OFFSET(-2), AUTO, AUTO, 0x00}, /**< SONYRF_ASCOT3_DTV_DVBC2_6   (DVB-C2 6MHzBW) */
    {0x00, AUTO, 0x03, 0x08, 0x08, 0x08, 0x02, 0x02, 0x02, 0x00, BW_8,  OFFSET(-2), OFFSET(0),  AUTO, AUTO, 0x00}, /**< SONYRF_ASCOT3_DTV_DVBC2_8   (DVB-C2 8MHzBW) */
    {0x00, AUTO, 0x04, 0x0A, 0x0A, 0x0A, 0x02, 0x02, 0x02, 0x00, BW_8,  OFFSET(2),  OFFSET(1),  AUTO, AUTO, 0x00}  /**< SONYRF_ASCOT3_DTV_DTMB      (DTMB) */
};

static const sonyrf_ascot3_adjust_param_t g_param_table_apmd_ascot3i[SONYRF_ASCOT3_TV_SYSTEM_NUM] = {
    /*
    OUTLMT    IF_BPF_GC                                           BW              BW_OFFSET         IF_OUT_SEL
      |  RF_GAIN  |     RFOVLD_DET_LV1    IFOVLD_DET_LV  IF_BPF_F0 |   FIF_OFFSET     |        AGC_SEL |  IS_LOWERLOCAL
      |     |     |    (VL)  (VH)  (U)   (VL)  (VH)  (U)    |      |       |          |          |     |     |          */
    {0x00, AUTO, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, BW_6,  OFFSET(0),  OFFSET(0),  AUTO, AUTO, 0x00}, /**< SONYRF_ASCOT3_TV_SYSTEM_UNKNOWN */
    /* Analog */
    {0x00, AUTO, 0x05, 0x03, 0x06, 0x03, 0x01, 0x01, 0x01, 0x00, BW_6,  OFFSET(0),  OFFSET(1),  AUTO, AUTO, 0x00}, /**< SONYRF_ASCOT3_ATV_MN_EIAJ   (System-M (Japan)) */
    {0x00, AUTO, 0x05, 0x03, 0x06, 0x03, 0x01, 0x01, 0x01, 0x00, BW_6,  OFFSET(0),  OFFSET(1),  AUTO, AUTO, 0x00}, /**< SONYRF_ASCOT3_ATV_MN_SAP    (System-M (US)) */
    {0x00, AUTO, 0x05, 0x03, 0x06, 0x03, 0x01, 0x01, 0x01, 0x00, BW_6,  OFFSET(3),  OFFSET(1),  AUTO, AUTO, 0x00}, /**< SONYRF_ASCOT3_ATV_MN_A2     (System-M (Korea)) */
    {0x00, AUTO, 0x05, 0x03, 0x06, 0x03, 0x01, 0x01, 0x01, 0x00, BW_7,  OFFSET(11), OFFSET(5),  AUTO, AUTO, 0x00}, /**< SONYRF_ASCOT3_ATV_BG        (System-B/G) */
    {0x00, AUTO, 0x05, 0x03, 0x06, 0x03, 0x01, 0x01, 0x01, 0x03, BW_8,  OFFSET(-2), OFFSET(4), AUTO, AUTO, 0x00}, /**< SONYRF_ASCOT3_ATV_I         (System-I) */
    {0x00, AUTO, 0x05, 0x03, 0x06, 0x03, 0x01, 0x01, 0x01, 0x00, BW_8,  OFFSET(2),  OFFSET(-3), AUTO, AUTO, 0x00}, /**< SONYRF_ASCOT3_ATV_DK        (System-D/K) */
    {0x00, AUTO, 0x05, 0x04, 0x0A, 0x00, 0x04, 0x04, 0x04, 0x00, BW_8,  OFFSET(2),  OFFSET(-3), AUTO, AUTO, 0x00}, /**< SONYRF_ASCOT3_ATV_L         (System-L) */
    {0x00, AUTO, 0x05, 0x04, 0x0A, 0x00, 0x04, 0x04, 0x04, 0x00, BW_8,  OFFSET(-1), OFFSET(4),  AUTO, AUTO, 0x00}, /**< SONYRF_ASCOT3_ATV_L_DASH    (System-L DASH) */
    /* Digital */
    {0x00, AUTO, 0x09, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x00, BW_6,  OFFSET(-2), OFFSET(4), AUTO, AUTO, 0x00}, /**< SONYRF_ASCOT3_DTV_8VSB      (ATSC 8VSB) */
    {0x00, AUTO, 0x09, 0x03, 0x03, 0x03, 0x02, 0x02, 0x02, 0x00, BW_6,  OFFSET(-2), OFFSET(4), AUTO, AUTO, 0x00}, /**< SONYRF_ASCOT3_DTV_QAM       (US QAM) */
    {0x00, AUTO, 0x09, 0x0B, 0x0B, 0x0B, 0x02, 0x02, 0x02, 0x00, BW_6,  OFFSET(-9), OFFSET(-5), AUTO, AUTO, 0x00}, /**< SONYRF_ASCOT3_DTV_ISDBT_6   (ISDB-T 6MHzBW) */
    {0x00, AUTO, 0x09, 0x0B, 0x0B, 0x0B, 0x02, 0x02, 0x02, 0x00, BW_7,  OFFSET(-7), OFFSET(-6), AUTO, AUTO, 0x00}, /**< SONYRF_ASCOT3_DTV_ISDBT_7   (ISDB-T 7MHzBW) */
    {0x00, AUTO, 0x09, 0x0B, 0x0B, 0x0B, 0x02, 0x02, 0x02, 0x00, BW_8,  OFFSET(-5), OFFSET(-7), AUTO, AUTO, 0x00}, /**< SONYRF_ASCOT3_DTV_ISDBT_8   (ISDB-T 8MHzBW) */
    {0x00, AUTO, 0x09, 0x0B, 0x0B, 0x0B, 0x02, 0x02, 0x02, 0x00, BW_6,  OFFSET(-8), OFFSET(-3), AUTO, AUTO, 0x00}, /**< SONYRF_ASCOT3_DTV_DVBT_5    (DVB-T 5MHzBW) */
    {0x00, AUTO, 0x09, 0x0B, 0x0B, 0x0B, 0x02, 0x02, 0x02, 0x00, BW_6,  OFFSET(-8), OFFSET(-3), AUTO, AUTO, 0x00}, /**< SONYRF_ASCOT3_DTV_DVBT_6    (DVB-T 6MHzBW) */
    {0x00, AUTO, 0x09, 0x0B, 0x0B, 0x0B, 0x02, 0x02, 0x02, 0x00, BW_7,  OFFSET(-6), OFFSET(-5), AUTO, AUTO, 0x00}, /**< SONYRF_ASCOT3_DTV_DVBT_7    (DVB-T 7MHzBW) */
    {0x00, AUTO, 0x09, 0x0B, 0x0B, 0x0B, 0x02, 0x02, 0x02, 0x00, BW_8,  OFFSET(-4), OFFSET(-6), AUTO, AUTO, 0x00}, /**< SONYRF_ASCOT3_DTV_DVBT_8    (DVB-T 8MHzBW) */
    {0x00, AUTO, 0x09, 0x0B, 0x0B, 0x0B, 0x02, 0x02, 0x02, 0x00, BW_1_7,OFFSET(-10),OFFSET(-10),AUTO, AUTO, 0x00}, /**< SONYRF_ASCOT3_DTV_DVBT2_1_7 (DVB-T2 1.7MHzBW) */
    {0x00, AUTO, 0x09, 0x0B, 0x0B, 0x0B, 0x02, 0x02, 0x02, 0x00, BW_6,  OFFSET(-8), OFFSET(-3), AUTO, AUTO, 0x00}, /**< SONYRF_ASCOT3_DTV_DVBT2_5   (DVB-T2 5MHzBW) */
    {0x00, AUTO, 0x09, 0x0B, 0x0B, 0x0B, 0x02, 0x02, 0x02, 0x00, BW_6,  OFFSET(-8), OFFSET(-3), AUTO, AUTO, 0x00}, /**< SONYRF_ASCOT3_DTV_DVBT2_6   (DVB-T2 6MHzBW) */
    {0x00, AUTO, 0x09, 0x0B, 0x0B, 0x0B, 0x02, 0x02, 0x02, 0x00, BW_7,  OFFSET(-6), OFFSET(-5), AUTO, AUTO, 0x00}, /**< SONYRF_ASCOT3_DTV_DVBT2_7   (DVB-T2 7MHzBW) */
    {0x00, AUTO, 0x09, 0x0B, 0x0B, 0x0B, 0x02, 0x02, 0x02, 0x00, BW_8,  OFFSET(-4), OFFSET(-6), AUTO, AUTO, 0x00}, /**< SONYRF_ASCOT3_DTV_DVBT2_8   (DVB-T2 8MHzBW) */
    {0x00, AUTO, 0x05, 0x02, 0x02, 0x02, 0x01, 0x01, 0x01, 0x00, BW_6,  OFFSET(-6), OFFSET(-4), AUTO, AUTO, 0x00}, /**< SONYRF_ASCOT3_DTV_DVBC_6    (DVB-C 6MHzBW) */
    {0x00, AUTO, 0x05, 0x02, 0x02, 0x02, 0x01, 0x01, 0x01, 0x00, BW_8,  OFFSET(-2), OFFSET(-3), AUTO, AUTO, 0x00}, /**< SONYRF_ASCOT3_DTV_DVBC_8    (DVB-C 8MHzBW) */
    {0x00, AUTO, 0x03, 0x09, 0x09, 0x09, 0x02, 0x02, 0x02, 0x00, BW_6,  OFFSET(-6), OFFSET(-2), AUTO, AUTO, 0x00}, /**< SONYRF_ASCOT3_DTV_DVBC2_6   (DVB-C2 6MHzBW) */
    {0x00, AUTO, 0x03, 0x09, 0x09, 0x09, 0x02, 0x02, 0x02, 0x00, BW_8,  OFFSET(-2), OFFSET(0),  AUTO, AUTO, 0x00}, /**< SONYRF_ASCOT3_DTV_DVBC2_8   (DVB-C2 8MHzBW) */
    {0x00, AUTO, 0x04, 0x0B, 0x0B, 0x0B, 0x02, 0x02, 0x02, 0x00, BW_8,  OFFSET(2),  OFFSET(1),  AUTO, AUTO, 0x00}  /**< SONYRF_ASCOT3_DTV_DTMB      (DTMB) */
};

/*------------------------------------------------------------------------------
  Structs
------------------------------------------------------------------------------*/

/**
 @brief ASCOT3 settings that may need to change depend on customer's system.
*/

typedef struct sonyrf_apmd_ascot3_module_param_t {
    uint32_t freq;
    int16_t  param;
} sonyrf_apmd_param_t;

typedef struct sonyrf_apmd_t {
	uint8_t                    tunerId;
	uint8_t                  lnastatus;
    const sonyrf_apmd_param_t* filter_DTV;
    const sonyrf_apmd_param_t* filter_ATV;
} sonyrf_apmd_t;


static const sonyrf_apmd_param_t DEFAULT[] = {
    {      0,  0 }
};

static const sonyrf_apmd_param_t FILTER_TYPEA[] = {
    {      0,  1 }
};

static const sonyrf_apmd_param_t FILTER_TYPEB[] = {
    { 464000,  1 },
    {      0,  0 }
};

static const sonyrf_apmd_param_t FILTER_TYPEC[] = {
    { 464000,  0 },
    {      0,  1 }
};

static const sonyrf_apmd_param_t RSSI_DE241[] = {
	{ 692000,   80 },
	{ 532000,  181 },
	{ 464000,  268 },
	{ 350000,   41 },
	{ 335000,   27 },
	{ 320000,  179 },
	{ 261000,  -22 },
	{ 215000,   32 },
	{ 184000,  171 },
	{ 172000,  300 },
	{ 150000,  271 },
	{ 122000,  214 },
	{  86000,  138 },
	{  65000,  194 },
	{  52000,  326 },
	{      0,  423 }
};

static const sonyrf_apmd_param_t RSSI_RE241[] = {
    { 692000,  145 },
    { 532000,  185 },
    { 464000,  317 },
    { 350000,   64 },
    { 335000,   44 },
    { 320000,   98 },
    { 261000,   40 },
    { 215000,   99 },
    { 184000,  238 },
    { 172000,  368 },
    { 150000,  327 },
    { 122000,  262 },
    {  86000,  190 },
    {  65000,  225 },
    {  52000,  425 },
    {      0,  605 },	
};

static const sonyrf_apmd_param_t RSSI_RA241[] = { // Based on RA231
    { 800000,  274 },
    { 630000,  251 },
    { 532000,  308 },
    { 464000,  243 },
    { 350000,  246 },
    { 320000,  129 },
    { 215000,  103 },
    { 196000,  -97 },
    { 184000,  210 },
    { 172000,  110 },
    { 150000,  210 },
    { 130000,  310 },
    {  65000,  193 },
    {  55000,  172 },
    {      0,  260 }
};

static const sonyrf_apmd_param_t RSSI_DB241[] = {
    { 692000,  223 },
    { 532000,  327 },
    { 464000,  453 },
    { 350000,  258 },
    { 335000,  236 },
    { 320000,  269 },
    { 261000,  166 },
    { 215000,  190 },
    { 184000,  306 },
    { 172000,  422 },
    { 150000,  420 },
    { 122000,  344 },
    {  86000,  252 },
    {  65000,  262 },
    {  52000,  434 },
    {      0,  580 }
};

static const sonyrf_apmd_param_t RSSI_DC241_LNAON[] = { // Based on RC232 LNAON
    { 692000,  517 },
    { 532000,  358 },
    { 464000,  388 },
    { 350000,  109 },
    { 335000,  100 },
    { 320000,  162 },
    { 261000,   87 },
    { 215000,  150 },
    { 184000,  298 },
    { 172000,  425 },
    { 150000,  403 },
    { 122000,  345 },
    {  86000,  229 },
    {  65000,  258 },
    {  52000,  572 },
    {      0,  885 }
};

static const sonyrf_apmd_param_t RSSI_DC241_LNAOFF[] = { // Based on RC232 LNAOFF
    { 692000,  775 },
    { 532000,  674 },
    { 464000,  800 },
    { 350000,  564 },
    { 335000,  550 },
    { 320000,  596 },
    { 261000,  520 },
    { 215000,  555 },
    { 184000,  678 },
    { 172000,  799 },
    { 150000,  797 },
    { 122000,  734 },
    {  86000,  624 },
    {  65000,  650 },
    {  52000,  855 },
    {      0, 1092 }
};

static const sonyrf_apmd_param_t RSSI_DB541_LNAON_1ST[] = {
    { 692000,  455 },
    { 532000,  378 },
    { 464000,  453 },
    { 350000,  307 },
    { 335000,  291 },
    { 320000,  335 },
    { 261000,  231 },
    { 215000,  283 },
    { 184000,  421 },
    { 172000,  543 },
    { 150000,  448 },
    { 122000,  385 },
    {  86000,  285 },
    {  65000,  307 },
    {  52000,  467 },
    {      0,  696 }
};

static const sonyrf_apmd_param_t RSSI_DB541_LNAON_2ND[] = {
    { 692000,  209 },
    { 532000,  235 },
    { 464000,  345 },
    { 350000,  173 },
    { 335000,  166 },
    { 320000,  225 },
    { 261000,  130 },
    { 215000,  210 },
    { 184000,  359 },
    { 172000,  493 },
    { 150000,  411 },
    { 122000,  358 },
    {  86000,  279 },
    {  65000,  345 },
    {  52000,  530 },
    {      0,  778 }
};

static const sonyrf_apmd_param_t RSSI_DB541_LNAOFF_1ST[] = {
    { 692000,  789 },
    { 532000,  797 },
    { 464000,  917 },
    { 350000,  832 },
    { 335000,  811 },
    { 320000,  845 },
    { 261000,  762 },
    { 215000,  785 },
    { 184000,  894 },
    { 172000, 1007 },
    { 150000,  932 },
    { 122000,  874 },
    {  86000,  782 },
    {  65000,  807 },
    {  52000,  950 },
    {      0, 1115 }
};

static const sonyrf_apmd_param_t RSSI_DB541_LNAOFF_2ND[] = {
    { 692000,  591 },
    { 532000,  643 },
    { 464000,  799 },
    { 350000,  699 },
    { 335000,  687 },
    { 320000,  737 },
    { 261000,  654 },
    { 215000,  688 },
    { 184000,  808 },
    { 172000,  921 },
    { 150000,  855 },
    { 122000,  793 },
    {  86000,  703 },
    {  65000,  730 },
    {  52000,  863 },
    {      0, 1032 }
};

static const sonyrf_apmd_param_t RSSI_DB541_CABLE[] = {
    { 692000,  183 },
    { 532000,  305 },
    { 464000,  459 },
    { 350000,  176 },
    { 335000,  164 },
    { 320000,  220 },
    { 261000,  130 },
    { 215000,  166 },
    { 184000,  295 },
    { 172000,  412 },
    { 150000,  329 },
    { 122000,  264 },
    {  86000,  171 },
    {  65000,  211 },
    {  52000,  389 },
    {      0,  622 }
};

static sonyrf_apmd_t g_Module;

/*--------------------------------------------------------------------
  Tuner instance creation

  sonyrf_ascot3_t   *pTuner      Tuner struct instance
  sonyrf_i2c_t       *pI2c        I2c struct instance
--------------------------------------------------------------------*/
sonyrf_result_t sonyrf_apmd_ascot3_module_Create(sonyrf_ascot3_t *pTuner, uint8_t i2cAddress, sonyrf_i2c_t *pI2c, uint8_t VariationID, uint8_t ProjectID)
{
	uint8_t  tmp;
    uint32_t config;
	sonyrf_ascot3_xtal_t xtal;
	
    sonyrf_result_t result = SONYRF_RESULT_OK;
    g_Module.tunerId       = VariationID;
    if(i2cAddress == 0xC0){
	    g_Module.lnastatus     = EXTLNA_OFF;
    }
    g_Module.filter_DTV    = DEFAULT;
    g_Module.filter_ATV    = DEFAULT;

    SONYRF_TRACE_ENTER("sonyrf_apmd_ascot3_module_Create");

    config = SONYRF_ASCOT3_CONFIG_REFOUT_500mVpp | SONYRF_ASCOT3_CONFIG_RFIN_MATCHING_ENABLE;

    if((!pTuner) || (!pI2c)){
        SONYRF_TRACE_RETURN(SONYRF_RESULT_ERROR_ARG);
    }

    if( i2cAddress == 0xC0 ){

    } else {
        config |= SONYRF_ASCOT3_CONFIG_EXT_REF;
    }
   
    switch(g_Module.tunerId){
		case MODULE_RE241:
		case MODULE_RA241:
		    xtal = SONYRF_ASCOT3_XTAL_16000KHz;
			break;
		case MODULE_DE241:
		case MODULE_RT241:
		case MODULE_DC241:
		case MODULE_DB241:
		case MODULE_DB541:
		default:
		    xtal = SONYRF_ASCOT3_XTAL_24000KHz;
			break;	
	}
				
    config |= (SONYRF_ASCOT3_CONFIG_OVERLOAD_STANDARD | SONYRF_ASCOT3_CONFIG_IFAGCSEL_ALL2);
    result = sonyrf_ascot3_Create(pTuner, xtal, i2cAddress ,pI2c, config );
    if(result != SONYRF_RESULT_OK){ SONYRF_TRACE_RETURN(result); }

    switch(g_Module.tunerId){		
		case MODULE_DC241:	
		    pTuner->xosc_cap_set = 0x1E;
			break;			
		case MODULE_RE241:
		case MODULE_RA241:
		    pTuner->xosc_cap_set = 0x64;
			break;
		case MODULE_DB541:
			pTuner->xosc_cap_set = 0x11;		
			break;			
		case MODULE_DE241:
		case MODULE_RT241:
		case MODULE_DB241:
		default:
		    pTuner->xosc_cap_set = 0x1E;
			break;			
	}

    switch(g_Module.tunerId){										
		case MODULE_RE241:
		case MODULE_DE241: 
		    g_Module.filter_DTV = FILTER_TYPEA; g_Module.filter_ATV = FILTER_TYPEC;
			break;	
		case MODULE_RA241:
		case MODULE_RT241: 
		case MODULE_DB241:       
		case MODULE_DB541:
		    g_Module.filter_DTV = FILTER_TYPEB; g_Module.filter_ATV = FILTER_TYPEB; 
			break;	
		case MODULE_DC241: 
		default:
			g_Module.filter_DTV = DEFAULT;      g_Module.filter_ATV = DEFAULT;      
			break;  	
    }
    
    result = pI2c->ReadRegister(pI2c, i2cAddress, 0x7F, &tmp, 1);
    if(result != SONYRF_RESULT_OK){ SONYRF_TRACE_RETURN(SONYRF_RESULT_ERROR_I2C); }
    if(tmp == 0xE1){
        pTuner->pParamTable  = g_param_table_apmd_ascot3i;
    }else{
        pTuner->pParamTable  = g_param_table_apmd_ascot3;
    }
	  
    SONYRF_TRACE_RETURN(SONYRF_RESULT_OK);
}

/*--------------------------------------------------------------------
  Tuner initialization

  sonyrf_ascot3_t   *pTuner      Tuner struct instance
--------------------------------------------------------------------*/
sonyrf_result_t sonyrf_apmd_ascot3_module_Initialize(sonyrf_ascot3_t *pTuner)
{
    sonyrf_result_t result = SONYRF_RESULT_OK;

    SONYRF_TRACE_ENTER("sonyrf_apmd_ascot3_module_Initialize");

    result = sonyrf_ascot3_Initialize(pTuner);
    if(result != SONYRF_RESULT_OK){ SONYRF_TRACE_RETURN(result); }

	if( (g_Module.tunerId == MODULE_DB541) && (pTuner->i2cAddress == 0xC0) ){
        result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x85, 0x01);
	    if(result != SONYRF_RESULT_OK){ SONYRF_TRACE_RETURN(result); }
	    g_Module.lnastatus     = EXTLNA_ON;
        result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x86, 0x01);
	    if(result != SONYRF_RESULT_OK){ SONYRF_TRACE_RETURN(result); }
    }
 
 	if( (g_Module.tunerId == MODULE_DC241) ){
        result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x85, 0x01);
	    if(result != SONYRF_RESULT_OK){ SONYRF_TRACE_RETURN(result); }
	    g_Module.lnastatus     = EXTLNA_ON;
    }
    result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x8b, 0x45);
	if(result != SONYRF_RESULT_OK){ SONYRF_TRACE_RETURN(result); }

    SONYRF_TRACE_RETURN(result);
}

/*--------------------------------------------------------------------
  Tuning

  sonyrf_ascot3_t          *pTuner      Tuner struct instance
  uint32_t                frequencykHz RF frequency (kHz)
  sonyrf_ascot_tv_system_t  system       Terrestrial broadcasting system ID
--------------------------------------------------------------------*/
sonyrf_result_t sonyrf_apmd_ascot3_module_Tune(sonyrf_ascot3_t *pTuner, uint32_t frequencykHz, sonyrf_ascot3_tv_system_t system)
{
    uint8_t ovrd_cnst;
	sonyrf_result_t result = SONYRF_RESULT_OK;
    uint8_t ExtCtrlEnabler;
    const sonyrf_apmd_param_t* table;
    uint8_t i = 0;
    
    SONYRF_TRACE_ENTER("sonyrf_apmd_ascot3_module_Tune"); 
    
    if((!pTuner) || (!pTuner->pI2c)){
        SONYRF_TRACE_RETURN(SONYRF_RESULT_ERROR_ARG);
    }

    if( SONYRF_ASCOT3_IS_DTV(system) ) { 
		table = g_Module.filter_DTV; 
		ovrd_cnst = 0x45;
	} else  { 
		table = g_Module.filter_ATV;
		ovrd_cnst = 0x01;
	}

    while( frequencykHz < table[i].freq  ){
        i++;
    }
    ExtCtrlEnabler = (uint8_t)table[i].param;

    pTuner->state = SONYRF_ASCOT3_STATE_ACTIVE;

	switch(system){
		case SONYRF_ASCOT3_DTV_8VSB:
		case SONYRF_ASCOT3_DTV_QAM:
		case SONYRF_ASCOT3_DTV_DVBC2_6:
		case SONYRF_ASCOT3_DTV_DVBC2_8:
		case SONYRF_ASCOT3_DTV_DTMB:
			pTuner->flags &= ~SONYRF_ASCOT3_CONFIG_LOOPFILTER_INTERNAL; // use external LoopFilter 
			break;
		default:
			pTuner->flags |=  SONYRF_ASCOT3_CONFIG_LOOPFILTER_INTERNAL; // use internal LoopFilter
			break;
	}

	if( g_Module.tunerId == MODULE_DC241 || g_Module.tunerId == MODULE_RE241 || g_Module.tunerId == MODULE_RA241 || g_Module.tunerId  == MODULE_RT241 ){
		
	    result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x84, 0x00);
		if(result != SONYRF_RESULT_OK){ SONYRF_TRACE_RETURN(result); }		

	}else{
#if 0
		switch(system){
			case SONYRF_ASCOT3_ATV_MN_SAP:
			case SONYRF_ASCOT3_ATV_MN_A2:
			case SONYRF_ASCOT3_ATV_MN_EIAJ:
			case SONYRF_ASCOT3_ATV_BG:
			case SONYRF_ASCOT3_ATV_I:
			case SONYRF_ASCOT3_ATV_DK:
			case SONYRF_ASCOT3_ATV_L:
			case SONYRF_ASCOT3_ATV_L_DASH:
//			    result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x84, 0x00);
//				if(result != SONYRF_RESULT_OK){ SONYRF_TRACE_RETURN(result); }
				break;
			default:
				result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x84, 0x80);
				if(result != SONYRF_RESULT_OK){ SONYRF_TRACE_RETURN(result); }
				break;
		}
#endif
	}

	result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x74, 0x12);
	if(result != SONYRF_RESULT_OK){ SONYRF_TRACE_RETURN(result); }

	result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x8B, ovrd_cnst);
	if(result != SONYRF_RESULT_OK){ SONYRF_TRACE_RETURN(result); }

    result = sonyrf_ascot3_SetRfExtCtrl(pTuner, ExtCtrlEnabler);
	if(result != SONYRF_RESULT_OK){ SONYRF_TRACE_RETURN(result); }

    result = sonyrf_ascot3_Tune(pTuner, frequencykHz, system);
	if(result != SONYRF_RESULT_OK){ SONYRF_TRACE_RETURN(result); }
    SONYRF_SLEEP(50);
    result = sonyrf_ascot3_TuneEnd(pTuner);
	if(result != SONYRF_RESULT_OK){ SONYRF_TRACE_RETURN(result); }

    SONYRF_TRACE_RETURN(result);
}

/*--------------------------------------------------------------------
  Tuner sleep

  sonyrf_ascot3_t   *pTuner      Tuner struct instance
--------------------------------------------------------------------*/


sonyrf_result_t sonyrf_apmd_ascot3_module_Sleep(sonyrf_ascot3_t *pTuner)
{
    sonyrf_result_t result = SONYRF_RESULT_OK;

    SONYRF_TRACE_ENTER("sonyrf_apmd_ascot3_module_Sleep");

	    result = sonyrf_ascot3_Sleep(pTuner);
	
	if(result != SONYRF_RESULT_OK){ SONYRF_TRACE_RETURN(result); }

	result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x84, 0x80);
	if(result != SONYRF_RESULT_OK){ SONYRF_TRACE_RETURN(result); }

    SONYRF_TRACE_RETURN(result);
}

/*--------------------------------------------------------------------
  Control Antenna SW

  sonyrf_ascot3_t   *pTuner      Tuner struct instance
  uint8_t          sw           Antenna Status (0:cable 1:Terr) 
--------------------------------------------------------------------*/
sonyrf_result_t sonyrf_apmd_ascot3_module_AntSw(sonyrf_ascot3_t *pTuner, uint8_t sw)
{
    sonyrf_result_t result = SONYRF_RESULT_OK;

    SONYRF_TRACE_ENTER("sonyrf_apmd_ascot3_module_AntSw");

    result = sonyrf_ascot3_SetGPO(pTuner, (uint8_t)1, sw);

    SONYRF_TRACE_RETURN(result);
}

/*--------------------------------------------------------------------
  Control External LNA

  sonyrf_ascot3_t   *pTuner      Tuner struct instance
  uint8_t         ena        LNA Status (0:OFF 1:ON) 
--------------------------------------------------------------------*/
sonyrf_result_t sonyrf_apmd_ascot3_module_ExtLna(sonyrf_ascot3_t *pTuner, uint8_t ena)
{
    sonyrf_result_t result = SONYRF_RESULT_OK;

    SONYRF_TRACE_ENTER("sonyrf_apmd_ascot3_module_ExtLna");

	if( pTuner->i2cAddress == 0xC0 ){
    result = sonyrf_ascot3_SetGPO(pTuner, (uint8_t)0, ena);
	    if(ena) g_Module.lnastatus = EXTLNA_ON;
		else    g_Module.lnastatus = EXTLNA_OFF;
	}

    SONYRF_TRACE_RETURN(result);
}

/*--------------------------------------------------------------------
  Shift frequency

  sonyrf_ascot3_t   *pTuner      Tuner struct instance
  uint32_t         frequencykHz RF frequency (kHz)
--------------------------------------------------------------------*/
sonyrf_result_t sonyrf_apmd_ascot3_module_Shift(sonyrf_ascot3_t *pTuner, uint32_t frequencykHz)
{
    sonyrf_result_t result = SONYRF_RESULT_OK;

    SONYRF_TRACE_ENTER("sonyrf_apmd_ascot3_module_Shift");

    result = sonyrf_ascot3_ShiftFRF(pTuner, frequencykHz);
	if(result != SONYRF_RESULT_OK){ SONYRF_TRACE_RETURN(result); }

    SONYRF_TRACE_RETURN(result);
}

/*--------------------------------------------------------------------
  Get signal strength

  sonyrf_ascot3_t   *pTuner      Tuner struct instance
--------------------------------------------------------------------*/
#define USER_OFFSET_X100 0

static sonyrf_result_t sonyrf_apmd_ascot3_module_ReadRssiManual(sonyrf_ascot3_t *pTuner, int32_t *pRssi);
static sonyrf_result_t apmd_X_rssi_Manual(sonyrf_ascot3_t *pTuner, uint8_t *pIFAGCReg, uint8_t *pRFAGCReg);

sonyrf_result_t sonyrf_apmd_ascot3_module_RSSI(sonyrf_ascot3_t *pTuner, int32_t *rssi_x100)
{

    sonyrf_result_t result = SONYRF_RESULT_OK;
    const sonyrf_apmd_param_t* table;
    const sonyrf_apmd_param_t* rssi_A;
    const sonyrf_apmd_param_t* rssi_B;
    const sonyrf_apmd_param_t* rssi_C;    	
    uint8_t i = 0;
    int16_t offset_x100;
    
    uint8_t data = 0xff;
    int16_t ifout_tgt;
    
    SONYRF_TRACE_ENTER("sonyrf_apmd_ascot3_module_RSSI");

    switch(g_Module.tunerId){					
		case MODULE_RE241: rssi_A = RSSI_RE241;            rssi_B = RSSI_RE241;           rssi_C = RSSI_RE241;        break;
		case MODULE_RA241: rssi_A = RSSI_RA241;            rssi_B = RSSI_RA241;           rssi_C = RSSI_RA241;        break;    
		case MODULE_DE241: rssi_A = RSSI_DE241;            rssi_B = RSSI_DE241;           rssi_C = RSSI_DE241;        break;
		case MODULE_RT241: rssi_A = RSSI_DB241;            rssi_B = RSSI_DB241;           rssi_C = RSSI_DB241;        break;
		case MODULE_DC241: rssi_A = RSSI_DC241_LNAOFF;     rssi_B = RSSI_DC241_LNAON;     rssi_C = RSSI_DC241_LNAOFF; break;
		case MODULE_DB241: rssi_A = RSSI_DB241;            rssi_B = RSSI_DB241;           rssi_C = RSSI_DB241;        break;           
		case MODULE_DB541: 	
		    if( pTuner->i2cAddress == 0xC0 ){
				           rssi_A = RSSI_DB541_LNAOFF_1ST; rssi_B = RSSI_DB541_LNAON_1ST; rssi_C = RSSI_DB541_CABLE;
		    }else{ // 0xC2
				           rssi_A = RSSI_DB541_LNAOFF_2ND; rssi_B = RSSI_DB541_LNAON_2ND; rssi_C = RSSI_DB541_CABLE;
			}
			break;
		default:           rssi_A = DEFAULT;               rssi_B = DEFAULT;              rssi_C = DEFAULT;           break;   	
	}

    if(g_Module.lnastatus == EXTLNA_ON){ table = rssi_B; }
    else                               { table = rssi_A; }

	if( (g_Module.tunerId == MODULE_DB541) && (pTuner->i2cAddress == 0xC0) ){
	    result = pTuner->pI2c->ReadRegister(pTuner->pI2c, pTuner->i2cAddress, 0x86, &data, 1);
	    if(result != SONYRF_RESULT_OK){ SONYRF_TRACE_RETURN(result); }
        if(data == 0x00)table = rssi_C; 		
	}
    
    while((pTuner->frequencykHz) < table[i].freq  ){ i++; }

    switch(pTuner->tvSystem)
    {
        case SONYRF_ASCOT3_DTV_8VSB:
            ifout_tgt = 0;
            break;
        case SONYRF_ASCOT3_DTV_QAM:
            ifout_tgt = 0;
            break;
        case SONYRF_ASCOT3_DTV_ISDBT_6:
        case SONYRF_ASCOT3_DTV_ISDBT_7:
        case SONYRF_ASCOT3_DTV_ISDBT_8:
            ifout_tgt = -450; // for CXD2838ER
            break;
        case SONYRF_ASCOT3_DTV_DVBT_5:
        case SONYRF_ASCOT3_DTV_DVBT_6:
        case SONYRF_ASCOT3_DTV_DVBT_7:
        case SONYRF_ASCOT3_DTV_DVBT_8:
            ifout_tgt = -400; // for CXD2837ER/CXD2841ER/CXD2842ER/CXD2843ER
            break;
        case SONYRF_ASCOT3_DTV_DVBT2_1_7:
        case SONYRF_ASCOT3_DTV_DVBT2_5:
        case SONYRF_ASCOT3_DTV_DVBT2_6:
        case SONYRF_ASCOT3_DTV_DVBT2_7:
        case SONYRF_ASCOT3_DTV_DVBT2_8:
            ifout_tgt = -400; // for CXD2837ER/CXD2841ER/CXD2842ER/CXD2843ER
            break;
        case SONYRF_ASCOT3_DTV_DVBC_6:
        case SONYRF_ASCOT3_DTV_DVBC_8:
            ifout_tgt = -550; // for CXD2837ER/CXD2841ER/CXD2842ER/CXD2843ER
            break;
        case SONYRF_ASCOT3_DTV_DVBC2_6:
        case SONYRF_ASCOT3_DTV_DVBC2_8:
            ifout_tgt = -400; // for CXD2837ER/CXD2841ER/CXD2842ER/CXD2843ER
            break;
        case SONYRF_ASCOT3_DTV_DTMB:
            ifout_tgt = -900; // for CXD2840ER
            break;
        default:
            ifout_tgt = 0;
            break;
    }

    offset_x100 = table[i].param + USER_OFFSET_X100 + ifout_tgt;

    if(offset_x100 > 1588)       offset_x100 =  1588;
    else if(offset_x100 < -1600) offset_x100 = -1600;
    
    result = sonyrf_apmd_ascot3_module_ReadRssiManual(pTuner, rssi_x100);
    if(result != SONYRF_RESULT_OK){ SONYRF_TRACE_RETURN(result); }

	*rssi_x100 += offset_x100;
    if((table != rssi_C) && (g_Module.lnastatus == EXTLNA_ON)) *rssi_x100 -= 1000; // Fixed offset at LNA ON.	

    SONYRF_TRACE_RETURN(SONYRF_RESULT_OK);
}

static sonyrf_result_t sonyrf_apmd_ascot3_module_ReadRssiManual(sonyrf_ascot3_t *pTuner, int32_t *pRssi)
{
    sonyrf_result_t result = SONYRF_RESULT_OK;
    uint8_t ifagcreg = 0;
    uint8_t rfagcreg = 0;
    int32_t ifgain =0;
    int32_t rfgain =0;

    if((!pTuner) || (!pRssi)){
        SONYRF_TRACE_RETURN(SONYRF_RESULT_ERROR_ARG);
    }

    SONYRF_TRACE_ENTER("sonyrf_apmd_ascot3_module_ReadRssiManual");

	if(result != SONYRF_RESULT_OK){ SONYRF_TRACE_RETURN(result); }

    if(pTuner->state != SONYRF_ASCOT3_STATE_ACTIVE){
        SONYRF_TRACE_RETURN(SONYRF_RESULT_ERROR_SW_STATE);
    }

    result = apmd_X_rssi_Manual(pTuner, &ifagcreg, &rfagcreg);
    if(result != SONYRF_RESULT_OK){
        SONYRF_TRACE_RETURN(result);
    }

    /*
        IFGAIN = if(AGC > 0.39){ 
                     8.7 + IF_BPF_GC 
                 }else{ 
                     8.7 + IF_BPF_GC + 76.9 * (0.39 - AGC) 
                 }
        Note that AGC(V) = IFAGCReg(by X_read_agc) * 1.4 / 255
        So...
        IFGAIN(100xdB) = if(IFAGCReg * 140 > 9945){
                             870 + (IF_BPF_GC * 100)
                         }else{
                             870 + (IF_BPF_GC * 100) + (769 * (9945 - IFAGCReg * 140)) / 2550
                         }
    */
	
    {
        const int32_t if_bpf_gc_table[] = {-6,-4,-2, 0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 20, 20};
        uint8_t data = 0;
        int32_t if_bpf_gc_x100 = 0;
        int32_t agcreg_x140 = ifagcreg * 140;

        result = pTuner->pI2c->ReadRegister(pTuner->pI2c, pTuner->i2cAddress, 0x69, &data, 1);
        if(result != SONYRF_RESULT_OK){ SONYRF_TRACE_RETURN(SONYRF_RESULT_ERROR_I2C); }

        if_bpf_gc_x100 = if_bpf_gc_table[data & 0x0F] * 100;

        if(agcreg_x140 > 9945){
            ifgain = 870 + if_bpf_gc_x100;
        }else{
            ifgain = 870 + if_bpf_gc_x100 + (769 * (9945 - agcreg_x140) + 1275) / 2550; /* Round */
        }
    }

    /*
        RFGAIN = if(max(AGC,RFAGC) < 0.192){
                     RFGAIN_MAX
                 }else if(max(AGC,RFAGC) < 0.214){
                     RF_GAIN_MAX - 70 * (max(AGC,RFAGC) - 0.192)
                 }else if(max(AGC,RFAGC) < 0.346){
                     RF_GAIN_MAX - 70 * 0.022
                 }else if(max(AGC,RFAGC) < 0.972){
                     RF_GAIN_MAX - 70 * 0.022 - 70 * (max(AGC,RFAGC) - 0.346)
                 }else if(max(AGC,RFAGC) < 1.18){
                     RF_GAIN_MAX - 70 * 0.648 - 57 * (max(AGC,RFAGC) - 0.972)
                 }else{
                     RF_GAIN_MAX - 70 * 0.648 - 57 * 0.208 - 160 * (max(AGC,RFAGC) - 1.18)
                 }
        Note that AGC(V) = IFAGCReg(by X_read_agc) * 1.4 / 255
                  RFAGC(V) = RFAGCReg(by X_read_ss) * 1.4 / 255
        So...
        RFGAIN(100xdB) = if(maxagcreg * 140 < 4896){
                             RFGAIN_MAX * 100
                         }else if(maxagcreg * 140 < 5457){
                             RFGAIN_MAX * 100 - (70 * (maxagcreg * 140 - 4896))/255
                         }else if(maxagcreg * 140 < 8823){
                             RFGAIN_MAX * 100 - 70 * 2.2
                         }else if(maxagcreg * 140 < 24786){
                             RFGAIN_MAX * 100 - 70 * 2.2 - (70 * (maxagcreg * 140 - 8823))/255
                         }else if(maxagcreg * 140 < 30090){
                             RFGAIN_MAX * 100 - 70 * 64.8 - (57 * (maxagcreg * 140 - 24786))/255
                         }else{
                             RFGAIN_MAX * 100 - 70 * 64.8 - 57 * 20.8 - 160 * (maxagcreg * 140 - 30090))/255
                         }
        (NOTE: maxagcreg = max(IFAGCReg, RFAGCReg))
    */
	
    {
        int32_t maxagcreg_x140 = 0;
        int32_t rfgainmax_x100 = 4200;

        if(ifagcreg > rfagcreg){
            maxagcreg_x140 = ifagcreg * 140;
        }else{
            maxagcreg_x140 = rfagcreg * 140;
        }

        if(maxagcreg_x140 <  4896 ){
            rfgain = rfgainmax_x100;
        }else if(maxagcreg_x140 <  5457 ){
            rfgain = rfgainmax_x100        - ( 70 * (maxagcreg_x140 -  4896) + 127) / 255; /* Round */
        }else if(maxagcreg_x140 <  8823 ){
            rfgain = rfgainmax_x100 -  154;
        }else if(maxagcreg_x140 < 24786 ){
            rfgain = rfgainmax_x100 -  154 - ( 70 * (maxagcreg_x140 -  8823) + 127) / 255; /* Round */
        }else if(maxagcreg_x140 < 30090 ){
            rfgain = rfgainmax_x100 - 4536 - ( 57 * (maxagcreg_x140 - 24786) + 127) / 255; /* Round */
        }else{
            rfgain = rfgainmax_x100 - 5722 - (160 * (maxagcreg_x140 - 30090) + 127) / 255; /* Round */
        }
    }
 
    *pRssi = - ifgain - rfgain;

    SONYRF_TRACE_RETURN(SONYRF_RESULT_OK);
}

/*--------------------------------------------------------------------
  Read IF gain and RF gain register to calculate RF signal strength

  sonyrf_ascot3_t  *pTuner      Tuner struct instance
  int32_t          *pIFAGCReg   IF block gain (100 x dB)
  int32_t          *pRFAGCReg   RF block gain (100 x dB)
--------------------------------------------------------------------*/
static sonyrf_result_t apmd_X_rssi_Manual(sonyrf_ascot3_t *pTuner, uint8_t *pIFAGCReg, uint8_t *pRFAGCReg)
{
    sonyrf_result_t result = SONYRF_RESULT_OK;

    SONYRF_TRACE_ENTER("apmd_X_rssi_Manual");

    if((!pTuner) || (!pTuner->pI2c) || (!pIFAGCReg) || (!pRFAGCReg)){
        SONYRF_TRACE_RETURN(SONYRF_RESULT_ERROR_ARG);
    }

    if((pTuner->state != SONYRF_ASCOT3_STATE_SLEEP) && (pTuner->state != SONYRF_ASCOT3_STATE_ACTIVE)){
        SONYRF_TRACE_RETURN(SONYRF_RESULT_ERROR_SW_STATE);
    }

    /* Logic wake up, CPU wake up */
    {
        const uint8_t cdata[2] = {0xC4, 0x41};
        result = pTuner->pI2c->WriteRegister(pTuner->pI2c, pTuner->i2cAddress, 0x87, cdata, sizeof(cdata));
        if(result != SONYRF_RESULT_OK){ SONYRF_TRACE_RETURN(SONYRF_RESULT_ERROR_I2C); }
    }

    /* Connect AGC,  ADC start  */
    {
        const uint8_t cdata[2] = {0x05, 0x01};
        result = pTuner->pI2c->WriteRegister(pTuner->pI2c, pTuner->i2cAddress, 0x59, cdata, sizeof(cdata));
        if(result != SONYRF_RESULT_OK){ SONYRF_TRACE_RETURN(SONYRF_RESULT_ERROR_I2C); }
    }

    /* ADC read out */
    result = pTuner->pI2c->ReadRegister(pTuner->pI2c, pTuner->i2cAddress, 0x5B, pIFAGCReg, 1);
    if(result != SONYRF_RESULT_OK){ SONYRF_TRACE_RETURN(SONYRF_RESULT_ERROR_I2C); }

    /* Connect AGC,  ADC start  */
    {
        const uint8_t cdata[2] = {0x03, 0x01};
        result = pTuner->pI2c->WriteRegister(pTuner->pI2c, pTuner->i2cAddress, 0x59, cdata, sizeof(cdata));
        if(result != SONYRF_RESULT_OK){ SONYRF_TRACE_RETURN(SONYRF_RESULT_ERROR_I2C); }
    }
    
    /* ADC read out */
    result = pTuner->pI2c->ReadRegister(pTuner->pI2c, pTuner->i2cAddress, 0x5B, pRFAGCReg, 1);
    if(result != SONYRF_RESULT_OK){ SONYRF_TRACE_RETURN(SONYRF_RESULT_ERROR_I2C); }

    /* Disconnect AGC */
    result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x59, 0x04);
    if(result != SONYRF_RESULT_OK){ SONYRF_TRACE_RETURN(SONYRF_RESULT_ERROR_I2C); }

    /* CPU deep sleep */
    result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x88, 0x00);
    if(result != SONYRF_RESULT_OK){ SONYRF_TRACE_RETURN(SONYRF_RESULT_ERROR_I2C); }

    /* Logic sleep */
    result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x87, 0xC0);
    if(result != SONYRF_RESULT_OK){ SONYRF_TRACE_RETURN(SONYRF_RESULT_ERROR_I2C); }

    SONYRF_TRACE_RETURN(SONYRF_RESULT_OK);

}
