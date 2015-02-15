/*------------------------------------------------------------------------------
  Copyright 2014 Sony Corporation

  Last Updated  : 2014/08/08
  File Revision : 1.9.0.2
------------------------------------------------------------------------------*/
#include "sonyrf_apmd_helene_module_forBRAVIA.h"

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

#define MODULE_CE241 (0x00)
#define MODULE_CE541 (0x01)

static const sonyrf_helene_terr_adjust_param_t g_terr_param_table_apmd_helene[SONYRF_HELENE_TERR_TV_SYSTEM_NUM] = {
    /*
         IF_BPF_GC                                           BW              BW_OFFSET
     RF_GAIN |     RFOVLD_DET_LV1    IFOVLD_DET_LV  IF_BPF_F0 |   FIF_OFFSET     |     IS_LOWERLOCAL
       |     |    (VL)  (VH)  (U)   (VL)  (VH)  (U)    |      |       |          |          |        */
    {AUTO, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, BW_6,  OFFSET(0),  OFFSET(0),  0x00}, /**< SONYRF_HELENE_TV_SYSTEM_UNKNOWN */
    /* Analog */
    {AUTO, 0x05, 0x03, 0x06, 0x03, 0x01, 0x01, 0x01, 0x00, BW_6,  OFFSET(0),  OFFSET(1),  0x00}, /**< SONYRF_HELENE_ATV_MN_EIAJ   (System-M (Japan)) */
    {AUTO, 0x05, 0x03, 0x06, 0x03, 0x01, 0x01, 0x01, 0x00, BW_6,  OFFSET(0),  OFFSET(1),  0x00}, /**< SONYRF_HELENE_ATV_MN_SAP    (System-M (US)) */
    {AUTO, 0x05, 0x03, 0x06, 0x03, 0x01, 0x01, 0x01, 0x00, BW_6,  OFFSET(3),  OFFSET(1),  0x00}, /**< SONYRF_HELENE_ATV_MN_A2     (System-M (Korea)) */
    {AUTO, 0x05, 0x03, 0x06, 0x03, 0x01, 0x01, 0x01, 0x00, BW_7,  OFFSET(11), OFFSET(5),  0x00}, /**< SONYRF_HELENE_ATV_BG        (System-B/G) */
    {AUTO, 0x05, 0x03, 0x06, 0x03, 0x01, 0x01, 0x01, 0x03, BW_8,  OFFSET(-2), OFFSET(4),  0x00}, /**< SONYRF_HELENE_ATV_I         (System-I) */
    {AUTO, 0x05, 0x03, 0x06, 0x03, 0x01, 0x01, 0x01, 0x00, BW_8,  OFFSET(2),  OFFSET(-3), 0x00}, /**< SONYRF_HELENE_ATV_DK        (System-D/K) */
    {AUTO, 0x05, 0x04, 0x0A, 0x04, 0x04, 0x04, 0x04, 0x00, BW_8,  OFFSET(2),  OFFSET(-3), 0x00}, /**< SONYRF_HELENE_ATV_L         (System-L) */
    {AUTO, 0x05, 0x04, 0x0A, 0x04, 0x04, 0x04, 0x04, 0x00, BW_8,  OFFSET(-1), OFFSET(4),  0x00}, /**< SONYRF_HELENE_ATV_L_DASH    (System-L DASH) */
    /* Digital */
    {AUTO, 0x09, 0x0B, 0x0B, 0x0B, 0x03, 0x03, 0x03, 0x00, BW_6,  OFFSET(-2), OFFSET(4),  0x00}, /**< SONYRF_HELENE_DTV_8VSB      (ATSC 8VSB) */
    {AUTO, 0x09, 0x0B, 0x0B, 0x0B, 0x02, 0x02, 0x02, 0x00, BW_6,  OFFSET(-2), OFFSET(4),  0x00}, /**< SONYRF_HELENE_DTV_QAM       (US QAM) */
    {AUTO, 0x09, 0x0B, 0x0B, 0x0B, 0x02, 0x02, 0x02, 0x00, BW_6,  OFFSET(-9), OFFSET(-5), 0x00}, /**< SONYRF_HELENE_DTV_ISDBT_6   (ISDB-T 6MHzBW) */
    {AUTO, 0x09, 0x0B, 0x0B, 0x0B, 0x02, 0x02, 0x02, 0x00, BW_7,  OFFSET(-7), OFFSET(-6), 0x00}, /**< SONYRF_HELENE_DTV_ISDBT_7   (ISDB-T 7MHzBW) */
    {AUTO, 0x09, 0x0B, 0x0B, 0x0B, 0x02, 0x02, 0x02, 0x00, BW_8,  OFFSET(-5), OFFSET(-7), 0x00}, /**< SONYRF_HELENE_DTV_ISDBT_8   (ISDB-T 8MHzBW) */
    {AUTO, 0x09, 0x0B, 0x0B, 0x0B, 0x02, 0x02, 0x02, 0x00, BW_6,  OFFSET(-8), OFFSET(-3), 0x00}, /**< SONYRF_HELENE_DTV_DVBT_5    (DVB-T 5MHzBW) */
    {AUTO, 0x09, 0x0B, 0x0B, 0x0B, 0x02, 0x02, 0x02, 0x00, BW_6,  OFFSET(-8), OFFSET(-3), 0x00}, /**< SONYRF_HELENE_DTV_DVBT_6    (DVB-T 6MHzBW) */
    {AUTO, 0x09, 0x0B, 0x0B, 0x0B, 0x02, 0x02, 0x02, 0x00, BW_7,  OFFSET(-6), OFFSET(-5), 0x00}, /**< SONYRF_HELENE_DTV_DVBT_7    (DVB-T 7MHzBW) */
    {AUTO, 0x09, 0x0B, 0x0B, 0x0B, 0x02, 0x02, 0x02, 0x00, BW_8,  OFFSET(-4), OFFSET(-6), 0x00}, /**< SONYRF_HELENE_DTV_DVBT_8    (DVB-T 8MHzBW) */
    {AUTO, 0x09, 0x0B, 0x0B, 0x0B, 0x02, 0x02, 0x02, 0x00, BW_1_7,OFFSET(-10),OFFSET(-10),0x00}, /**< SONYRF_HELENE_DTV_DVBT2_1_7 (DVB-T2 1.7MHzBW) */
    {AUTO, 0x09, 0x0B, 0x0B, 0x0B, 0x02, 0x02, 0x02, 0x00, BW_6,  OFFSET(-8), OFFSET(-3), 0x00}, /**< SONYRF_HELENE_DTV_DVBT2_5   (DVB-T2 5MHzBW) */
    {AUTO, 0x09, 0x0B, 0x0B, 0x0B, 0x02, 0x02, 0x02, 0x00, BW_6,  OFFSET(-8), OFFSET(-3), 0x00}, /**< SONYRF_HELENE_DTV_DVBT2_6   (DVB-T2 6MHzBW) */
    {AUTO, 0x09, 0x0B, 0x0B, 0x0B, 0x02, 0x02, 0x02, 0x00, BW_7,  OFFSET(-6), OFFSET(-5), 0x00}, /**< SONYRF_HELENE_DTV_DVBT2_7   (DVB-T2 7MHzBW) */
    {AUTO, 0x09, 0x0B, 0x0B, 0x0B, 0x02, 0x02, 0x02, 0x00, BW_8,  OFFSET(-4), OFFSET(-6), 0x00}, /**< SONYRF_HELENE_DTV_DVBT2_8   (DVB-T2 8MHzBW) */
    {AUTO, 0x05, 0x02, 0x02, 0x02, 0x01, 0x01, 0x01, 0x00, BW_6,  OFFSET(-6), OFFSET(-4), 0x00}, /**< SONYRF_HELENE_DTV_DVBC_6    (DVB-C 6MHzBW) */
    {AUTO, 0x05, 0x02, 0x02, 0x02, 0x01, 0x01, 0x01, 0x00, BW_8,  OFFSET(-2), OFFSET(-3), 0x00}, /**< SONYRF_HELENE_DTV_DVBC_8    (DVB-C 8MHzBW) */
    {AUTO, 0x03, 0x09, 0x09, 0x09, 0x02, 0x02, 0x02, 0x00, BW_6,  OFFSET(-6), OFFSET(-2), 0x00}, /**< SONYRF_HELENE_DTV_DVBC2_6   (DVB-C2 6MHzBW) */
    {AUTO, 0x03, 0x09, 0x09, 0x09, 0x02, 0x02, 0x02, 0x00, BW_8,  OFFSET(-2), OFFSET(0),  0x00}, /**< SONYRF_HELENE_DTV_DVBC2_8   (DVB-C2 8MHzBW) */
    {AUTO, 0x04, 0x0B, 0x0B, 0x0B, 0x02, 0x02, 0x02, 0x00, BW_8,  OFFSET(2),  OFFSET(1),  0x00}  /**< SONYRF_HELENE_DTV_DTMB      (DTMB) */
};


/*------------------------------------------------------------------------------
  Structs
------------------------------------------------------------------------------*/

/**
 @brief HELENE settings that may need to change depend on customer's system.
*/

typedef struct sonyrf_apmd_helene_module_param_t {
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

static const sonyrf_apmd_param_t RSSI_CE541_LNAON_1ST[] = {
    { 692000, 580 },
    { 532000, 554 },
    { 464000, 645 },
    { 350000, 424 },
    { 335000, 409 },
    { 320000, 468 },
    { 261000, 356 },
    { 215000, 418 },
    { 184000, 566 },
    { 172000, 696 },
    { 150000, 766 },
    { 122000, 674 },
    {  86000, 577 },
    {  65000, 618 },
    {  52000, 803 },
    {      0,1073 }
};

static const sonyrf_apmd_param_t RSSI_CE541_LNAOFF_1ST[] = { //Tentative
    { 692000, 950 },
    { 532000, 969 },
    { 464000,1087 },
    { 350000, 913 },
    { 335000, 897 },
    { 320000, 942 },
    { 261000, 846 },
    { 215000, 886 },
    { 184000,1005 },
    { 172000,1125 },
    { 150000,1180 },
    { 122000,1122 },
    {  86000,1067 },
    {  65000,1109 },
    {  52000,1282 },
    {      0,1530 }
};

static const sonyrf_apmd_param_t RSSI_CE541_LNAON_2ND[] = {
    { 692000, 586 },
    { 532000, 650 },
    { 464000, 797 },
    { 350000, 559 },
    { 335000, 542 },
    { 320000, 598 },
    { 261000, 595 },
    { 215000, 577 },
    { 184000, 700 },
    { 172000, 828 },
    { 150000, 916 },
    { 122000, 833 },
    {  86000, 699 },
    {  65000, 669 },
    {  52000, 815 },
    {      0, 991 }
};

static const sonyrf_apmd_param_t RSSI_CE541_LNAOFF_2ND[] = { //tentative
    { 692000, 860 },
    { 532000,1035 },
    { 464000,1116 },
    { 350000, 932 },
    { 335000, 917 },
    { 320000, 965 },
    { 261000, 882 },
    { 215000, 921 },
    { 184000,1040 },
    { 172000,1154 },
    { 150000,1207 },
    { 122000,1155 },
    {  86000,1084 },
    {  65000,1118 },
    {  52000,1308 },
    {      0,1570 }
};

static const sonyrf_apmd_param_t RSSI_CE241[] = { 
    { 692000,  63 },
    { 532000, 101 },
    { 464000, 225 },
    { 350000,  -4 },
    { 335000, -25 },
    { 320000,  34 },
    { 261000, -70 },
    { 215000, -16 },
    { 184000, 127 },
    { 172000, 262 },
    { 150000, 305 },
    { 122000, 234 },
    {  86000, 136 },
    {  65000, 180 },
    {  52000, 374 },
    {      0, 556 } 
};


static sonyrf_apmd_t g_Module;

/*--------------------------------------------------------------------
  Tuner instance creation

  sonyrf_helene_t   *pTuner      Tuner struct instance
  sonyrf_i2c_t    *pI2c        I2c struct instance
--------------------------------------------------------------------*/
sonyrf_result_t sonyrf_apmd_helene_module_Create(sonyrf_helene_t *pTuner, uint8_t i2cAddress, sonyrf_i2c_t *pI2c, uint8_t VariationID, uint8_t ProjectID)
{
	uint32_t config;
	
    sonyrf_result_t result = SONYRF_RESULT_OK;
    g_Module.tunerId       = VariationID;
    if(i2cAddress == 0xC0){
	    g_Module.lnastatus     = EXTLNA_OFF;
    }
    g_Module.filter_DTV    = DEFAULT;
    g_Module.filter_ATV    = DEFAULT;

    SONYRF_TRACE_ENTER("sonyrf_apmd_helene_module_Create");

    config = SONYRF_HELENE_CONFIG_REFOUT_500mVpp | SONYRF_HELENE_CONFIG_POWERSAVE_TERR_RFIN_MATCHING;

    if((!pTuner) || (!pI2c)){
        SONYRF_TRACE_RETURN(SONYRF_RESULT_ERROR_ARG);
    }

    if( i2cAddress == 0xC0 ){
//        result = pI2c->WriteOneRegister(pI2c, i2cAddress, 0x86, 0x10);
//        if(result != SONYRF_RESULT_OK){ SONYRF_TRACE_RETURN(SONYRF_RESULT_ERROR_I2C); }
//        result = pI2c->ReadRegister(pI2c, i2cAddress, 0x4B, &tmp, 1);
//        if(result != SONYRF_RESULT_OK){ SONYRF_TRACE_RETURN(SONYRF_RESULT_ERROR_I2C); }
//        if(tmp == 0x00){
//            g_Module.tunerId = MODULE_CE241;
//        }else{
//            g_Module.tunerId = MODULE_CE541;
//        }
    } else {
//        g_Module.tunerId = MODULE_CE541;
        config |= SONYRF_HELENE_CONFIG_EXT_REF;
    }
   
    config |= SONYRF_HELENE_CONFIG_OVERLOAD_STANDARD;
    if(g_Module.tunerId == MODULE_CE541) config |= SONYRF_HELENE_CONFIG_SAT_LNA_OFF;

    result = sonyrf_helene_Create(pTuner, SONYRF_HELENE_XTAL_24000KHz, i2cAddress ,pI2c, config );

	if(result != SONYRF_RESULT_OK){ SONYRF_TRACE_RETURN(result); }
	
	if(g_Module.tunerId == MODULE_CE241){
		pTuner->xosc_cap_set = 0x1E;
	}else{
	    pTuner->xosc_cap_set = 0x11;
	}
	
    g_Module.filter_DTV = FILTER_TYPEA;
    g_Module.filter_ATV = FILTER_TYPEC;
   
	pTuner->pTerrParamTable  = g_terr_param_table_apmd_helene;
	pTuner->isFreesatMode = 1; 
	
    SONYRF_TRACE_RETURN(SONYRF_RESULT_OK);
}

/*--------------------------------------------------------------------
  Tuner initialization

  sonyrf_helene_t   *pTuner      Tuner struct instance
--------------------------------------------------------------------*/
sonyrf_result_t sonyrf_apmd_helene_module_Initialize( sonyrf_helene_t *pTuner )
{
    sonyrf_result_t result = SONYRF_RESULT_OK;

    SONYRF_TRACE_ENTER("sonyrf_apmd_helene_module_Initialize");

	result = sonyrf_helene_Initialize(pTuner);
    if(result != SONYRF_RESULT_OK){ SONYRF_TRACE_RETURN(result); }

	if( (g_Module.tunerId == MODULE_CE541) && (pTuner->i2cAddress == 0xC0) ){
        result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x85, 0x01);
	    if(result != SONYRF_RESULT_OK){ SONYRF_TRACE_RETURN(result); }
	    g_Module.lnastatus     = EXTLNA_ON;
    }

    result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x86, 0x10);
	if(result != SONYRF_RESULT_OK){ SONYRF_TRACE_RETURN(result); }
    result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x8b, 0x45);
	if(result != SONYRF_RESULT_OK){ SONYRF_TRACE_RETURN(result); }
    result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x43, 0x05);
	if(result != SONYRF_RESULT_OK){ SONYRF_TRACE_RETURN(result); }

    SONYRF_TRACE_RETURN(result);
}

/*--------------------------------------------------------------------
  Tuning

  sonyrf_helene_t          *pTuner      Tuner struct instance
  uint32_t                frequencykHz RF frequency (kHz)
  sonyrf_ascot_tv_system_t  system       Terrestrial broadcasting system ID
--------------------------------------------------------------------*/
sonyrf_result_t sonyrf_apmd_helene_module_terr_Tune(sonyrf_helene_t *pTuner, uint32_t frequencykHz, sonyrf_helene_tv_system_t system)
{
    uint8_t ovrd_cnst;
	sonyrf_result_t result = SONYRF_RESULT_OK;
    uint8_t ExtCtrlEnabler;
    const sonyrf_apmd_param_t* table;
    uint8_t i = 0;
    
    SONYRF_TRACE_ENTER("sonyrf_apmd_helene_module_terr_Tune");
    
    if((!pTuner) || (!pTuner->pI2c)){
        SONYRF_TRACE_RETURN(SONYRF_RESULT_ERROR_ARG);
    }
 
    if( SONYRF_HELENE_IS_DTV(system) ) { 
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

	pTuner->state = SONYRF_HELENE_STATE_ACTIVE_T;
	
	switch(system){
		case SONYRF_HELENE_DTV_8VSB:
		case SONYRF_HELENE_DTV_QAM:
		case SONYRF_HELENE_DTV_DVBC2_6:
		case SONYRF_HELENE_DTV_DVBC2_8:
		case SONYRF_HELENE_DTV_DTMB:
			pTuner->flags &= ~SONYRF_HELENE_CONFIG_LOOPFILTER_INTERNAL; // use external LoopFilter 
			break;
		default:
			pTuner->flags |=  SONYRF_HELENE_CONFIG_LOOPFILTER_INTERNAL; // use internal LoopFilter
			break;
	}

#if 0
	switch(system){
		case SONYRF_HELENE_ATV_MN_SAP:
		case SONYRF_HELENE_ATV_MN_A2:
		case SONYRF_HELENE_ATV_MN_EIAJ:
		case SONYRF_HELENE_ATV_BG:
		case SONYRF_HELENE_ATV_I:
		case SONYRF_HELENE_ATV_DK:
		case SONYRF_HELENE_ATV_L:
		case SONYRF_HELENE_ATV_L_DASH:
	        result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x84, 0x00);
		    if(result != SONYRF_RESULT_OK){ SONYRF_TRACE_RETURN(result); }
			break;
		default:
			result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x84, 0x80);
			if(result != SONYRF_RESULT_OK){ SONYRF_TRACE_RETURN(result); }
			break;
	}
#endif

	result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x74, 0x12);
	if(result != SONYRF_RESULT_OK){ SONYRF_TRACE_RETURN(result); }

	result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x8B, ovrd_cnst);
	if(result != SONYRF_RESULT_OK){ SONYRF_TRACE_RETURN(result); }

	result = sonyrf_helene_SetRfExtCtrl(pTuner, ExtCtrlEnabler);
	if(result != SONYRF_RESULT_OK){ SONYRF_TRACE_RETURN(result); }

	result = sonyrf_helene_terr_Tune(pTuner, frequencykHz, system);
	if(result != SONYRF_RESULT_OK){ SONYRF_TRACE_RETURN(result); }
	SONYRF_SLEEP(50);
	result = sonyrf_helene_terr_TuneEnd(pTuner);
	if(result != SONYRF_RESULT_OK){ SONYRF_TRACE_RETURN(result); }

    SONYRF_TRACE_RETURN(result);
}


/*--------------------------------------------------------------------
  Tuning

  sonyrf_apmd_t          *pTuner      Tuner struct instance
  uint32_t                frequencykHz RF frequency (kHz)
  sonyrf_ascot_tv_system_t  system       Terrestrial broadcasting system ID
--------------------------------------------------------------------*/
sonyrf_result_t sonyrf_apmd_helene_module_sat_Tune(sonyrf_helene_t *pTuner, uint32_t frequencykHz, sonyrf_helene_tv_system_t system, uint32_t symbolRateksps )
{
    sonyrf_result_t result = SONYRF_RESULT_OK;
    
    SONYRF_TRACE_ENTER("sonyrf_apmd_helene_module_sat_Tune");

	pTuner->state = SONYRF_HELENE_STATE_ACTIVE_S;
	
	switch(system){
		case SONYRF_HELENE_STV_ISDBS:	
	    pTuner->flags |=  SONYRF_HELENE_CONFIG_LOOPFILTER_INTERNAL; // use internal LoopFilter
			break;
		case SONYRF_HELENE_STV_DVBS:	
		case SONYRF_HELENE_STV_DVBS2:
		default:
	    pTuner->flags &= ~SONYRF_HELENE_CONFIG_LOOPFILTER_INTERNAL; // use external LoopFilter 			
			 break;
	}
	
    result = sonyrf_helene_sat_Tune(pTuner, frequencykHz, system, symbolRateksps);
	if(result != SONYRF_RESULT_OK){ SONYRF_TRACE_RETURN(result); }

    SONYRF_TRACE_RETURN(result);
}

/*--------------------------------------------------------------------
  Tuner sleep

  sonyrf_helene_t   *pTuner      Tuner struct instance
--------------------------------------------------------------------*/

sonyrf_result_t sonyrf_apmd_helene_module_Sleep(sonyrf_helene_t *pTuner)
{
	sonyrf_result_t result = SONYRF_RESULT_OK;
	sonyrf_helene_tv_system_t system;

    SONYRF_TRACE_ENTER("sonyrf_apmd_helene_module_Sleep");

	system = pTuner->tvSystem;

    	result = sonyrf_helene_Sleep(pTuner);
	
	if(result != SONYRF_RESULT_OK){ SONYRF_TRACE_RETURN(result); }

	if( SONYRF_HELENE_IS_STV(system)){
		result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x43, 0x05);
		if(result != SONYRF_RESULT_OK){ SONYRF_TRACE_RETURN(result); }
	}else{
		result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x61, 0x67);
		if(result != SONYRF_RESULT_OK){ SONYRF_TRACE_RETURN(result); }
		result = pTuner->pI2c->WriteOneRegister(pTuner->pI2c, pTuner->i2cAddress, 0x84, 0x80);
		if(result != SONYRF_RESULT_OK){ SONYRF_TRACE_RETURN(result); }
	}

    SONYRF_TRACE_RETURN(result);
}

/*--------------------------------------------------------------------
  Control Antenna SW

  sonyrf_helene_t    *pTuner      Tuner struct instance
  uint8_t          sw           Antenna Status (0:cable 1:Terr) 
--------------------------------------------------------------------*/
sonyrf_result_t sonyrf_apmd_helene_module_terrAntSw(sonyrf_helene_t *pTuner, uint8_t sw)
{
    sonyrf_result_t result = SONYRF_RESULT_OK;

    SONYRF_TRACE_ENTER("sonyrf_apmd_helene_module_AntSw");

    /* not used */

    SONYRF_TRACE_RETURN(result);
}

sonyrf_result_t sonyrf_apmd_helene_module_satAntSw(sonyrf_helene_t *pTuner2nd, uint8_t sw)
{
    sonyrf_result_t result = SONYRF_RESULT_OK;

    SONYRF_TRACE_ENTER("sonyrf_apmd_helene_module_AntSw");

	if( pTuner2nd->i2cAddress == 0xC2 ){
    result = sonyrf_helene_SetGPO(pTuner2nd, (uint8_t)0, sw);
	}

    SONYRF_TRACE_RETURN(result);
}

/*--------------------------------------------------------------------
  Control External LNA

  sonyrf_helene_t  *pTuner   Tuner struct instance of 1st Tuner
  uint8_t          ena       LNA Status (0:OFF 1:ON) 
--------------------------------------------------------------------*/
sonyrf_result_t sonyrf_apmd_helene_module_TerrExtLna(sonyrf_helene_t *pTuner, uint8_t ena)
{
    sonyrf_result_t result = SONYRF_RESULT_OK;

    SONYRF_TRACE_ENTER("sonyrf_apmd_helene_module_ExtLna");

	if( pTuner->i2cAddress == 0xC0 ){
        result = sonyrf_helene_SetGPO(pTuner, (uint8_t)0, ena);
	    if(ena) g_Module.lnastatus = EXTLNA_ON;
		else    g_Module.lnastatus = EXTLNA_OFF;
	}

    SONYRF_TRACE_RETURN(result);
}

/*--------------------------------------------------------------------
  Shift frequency

  sonyrf_helene_t   *pTuner      Tuner struct instance
  uint32_t         frequencykHz RF frequency (kHz)
--------------------------------------------------------------------*/
sonyrf_result_t sonyrf_apmd_helene_module_Shift(sonyrf_helene_t *pTuner, uint32_t frequencykHz)
{
    sonyrf_result_t result = SONYRF_RESULT_OK;

    SONYRF_TRACE_ENTER("sonyrf_apmd_helene_module_Shift");

	result = sonyrf_helene_ShiftFRF(pTuner, frequencykHz);
	if(result != SONYRF_RESULT_OK){ SONYRF_TRACE_RETURN(result); }

    SONYRF_TRACE_RETURN(result);
}

/*--------------------------------------------------------------------
  Get signal strength

  sonyrf_helene_t   *pTuner      Tuner struct instance
--------------------------------------------------------------------*/
#define USER_OFFSET_X100 0

static sonyrf_result_t sonyrf_apmd_helene_module_ReadRssiManual(sonyrf_helene_t *pTuner, int32_t *pRssi);
static sonyrf_result_t apmd_X_rssi_Manual(sonyrf_helene_t *pTuner, uint8_t *pIFAGCReg, uint8_t *pRFAGCReg);

sonyrf_result_t sonyrf_apmd_helene_module_RSSI(sonyrf_helene_t *pTuner, int32_t *rssi_x100)
{

    sonyrf_result_t result = SONYRF_RESULT_OK;
    const sonyrf_apmd_param_t* table;
    const sonyrf_apmd_param_t* rssi_A;
    const sonyrf_apmd_param_t* rssi_B;
    const sonyrf_apmd_param_t* rssi_C;    
    uint8_t i = 0;
    int16_t offset_x100;

    int16_t ifout_tgt;

    SONYRF_TRACE_ENTER("sonyrf_apmd_helene_module_RSSI");

	if((g_Module.tunerId == MODULE_CE541) && (pTuner->i2cAddress == 0xC0) ){
	    rssi_A = RSSI_CE541_LNAOFF_1ST; rssi_B = RSSI_CE541_LNAON_1ST; rssi_C = DEFAULT;
    }else if((g_Module.tunerId == MODULE_CE541) && (pTuner->i2cAddress == 0xC2) ){
	    rssi_A = RSSI_CE541_LNAOFF_2ND; rssi_B = RSSI_CE541_LNAON_2ND; rssi_C = DEFAULT;
    } else {
	    rssi_A = RSSI_CE241;            rssi_B = RSSI_CE241;           rssi_C = DEFAULT;
    }

    if(g_Module.lnastatus == EXTLNA_ON){ table = rssi_B; }
    else                               { table = rssi_A; }
 
    while((pTuner->frequencykHz) < table[i].freq  ){ i++; }

    switch(pTuner->tvSystem)
    {
        case SONYRF_HELENE_DTV_8VSB:
            ifout_tgt = 0;
            break;
        case SONYRF_HELENE_DTV_QAM:
            ifout_tgt = 0;
            break;
        case SONYRF_HELENE_DTV_ISDBT_6:
        case SONYRF_HELENE_DTV_ISDBT_7:
        case SONYRF_HELENE_DTV_ISDBT_8:
            ifout_tgt = -450; // for CXD2838ER
            break;
        case SONYRF_HELENE_DTV_DVBT_5:
        case SONYRF_HELENE_DTV_DVBT_6:
        case SONYRF_HELENE_DTV_DVBT_7:
        case SONYRF_HELENE_DTV_DVBT_8:
            ifout_tgt = -400; // for CXD2837ER/CXD2841ER/CXD2842ER/CXD2843ER
            break;
        case SONYRF_HELENE_DTV_DVBT2_1_7:
        case SONYRF_HELENE_DTV_DVBT2_5:
        case SONYRF_HELENE_DTV_DVBT2_6:
        case SONYRF_HELENE_DTV_DVBT2_7:
        case SONYRF_HELENE_DTV_DVBT2_8:
            ifout_tgt = -400; // for CXD2837ER/CXD2841ER/CXD2842ER/CXD2843ER
            break;
        case SONYRF_HELENE_DTV_DVBC_6:
        case SONYRF_HELENE_DTV_DVBC_8:
            ifout_tgt = -550; // for CXD2837ER/CXD2841ER/CXD2842ER/CXD2843ER
            break;
        case SONYRF_HELENE_DTV_DVBC2_6:
        case SONYRF_HELENE_DTV_DVBC2_8:
            ifout_tgt = -400; // for CXD2837ER/CXD2841ER/CXD2842ER/CXD2843ER
            break;
        case SONYRF_HELENE_DTV_DTMB:
            ifout_tgt = -900; // for CXD2840ER
            break;
        default:
            ifout_tgt = 0;
            break;
    }

    offset_x100 = table[i].param + USER_OFFSET_X100 + ifout_tgt;

    if(offset_x100 > 1588)       offset_x100 =  1588;
    else if(offset_x100 < -1600) offset_x100 = -1600;
    
    result = sonyrf_apmd_helene_module_ReadRssiManual(pTuner, rssi_x100);
    if(result != SONYRF_RESULT_OK){ SONYRF_TRACE_RETURN(result); }

	*rssi_x100 += offset_x100;
    if((table != rssi_C) && (g_Module.lnastatus == EXTLNA_ON)) *rssi_x100 -= 1000; // Fixed offset at LNA ON.	

    SONYRF_TRACE_RETURN(SONYRF_RESULT_OK);
}

static sonyrf_result_t sonyrf_apmd_helene_module_ReadRssiManual(sonyrf_helene_t *pTuner, int32_t *pRssi)
{
    sonyrf_result_t result = SONYRF_RESULT_OK;
    uint8_t ifagcreg = 0;
    uint8_t rfagcreg = 0;
    int32_t ifgain =0;
    int32_t rfgain =0;

    if((!pTuner) || (!pRssi)){
        SONYRF_TRACE_RETURN(SONYRF_RESULT_ERROR_ARG);
    }

    SONYRF_TRACE_ENTER("sonyrf_apmd_helene_module_ReadRssiManual");

	if(result != SONYRF_RESULT_OK){ SONYRF_TRACE_RETURN(result); }

    if(pTuner->state != SONYRF_HELENE_STATE_ACTIVE_T){
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
        int32_t rfgainmax_x100 = 4150;

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

  sonyrf_helene_t  *pTuner      Tuner struct instance
  int32_t          *pIFAGCReg   IF block gain (100 x dB)
  int32_t          *pRFAGCReg   RF block gain (100 x dB)
--------------------------------------------------------------------*/
static sonyrf_result_t apmd_X_rssi_Manual(sonyrf_helene_t *pTuner, uint8_t *pIFAGCReg, uint8_t *pRFAGCReg)
{
    sonyrf_result_t result = SONYRF_RESULT_OK;

    SONYRF_TRACE_ENTER("apmd_X_rssi_Manual");

    if((!pTuner) || (!pTuner->pI2c) || (!pIFAGCReg) || (!pRFAGCReg)){
        SONYRF_TRACE_RETURN(SONYRF_RESULT_ERROR_ARG);
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

