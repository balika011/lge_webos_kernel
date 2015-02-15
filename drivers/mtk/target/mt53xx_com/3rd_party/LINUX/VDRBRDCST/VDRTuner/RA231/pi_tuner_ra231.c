/*
  NOTE: This is glue layer code of RA231 driver but not perfect
  because Sony do not have much information about MTK software.
  Please modify it to fit MTK's system.
*/

#include "sonyrf_ascot3es1_ra231.h"
#include "sonyrf_ascot3_ra231.h"
#include "sonyrf_i2c_MTK.h"

#ifndef SONYRF_DUMMY_DEBUG // For checking driver in Sony dummy environment. Please ignore it.
#include "tuner_interface_if.h"
#include "fe_tuner_common_if.h"
#include "i2c_api.h"
#include "x_typedef.h"
#include "tunerDebug.h"
#include "x_os.h"
#include "eq_script_ra231.h"
#else // SONYRF_DUMMY_DEBUG
#include "MTKdef.h"
#endif

//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------
//Frequency boundary .Get from tuner spec
#define C_RA231_LO_ADDRESS           0xC0 // In 8bit form

#define C_RA231_FREQ_DBOUND_UPPER      1002*1000*1000
#define C_RA231_FREQ_DBOUND_LOWER        42*1000*1000
#define C_RA231_FREQ_DBOUND_UPPER_Ana  1002*1000*1000
#define C_RA231_FREQ_DBOUND_LOWER_Ana    42*1000*1000

#define C_RA231_IF_FREQUENCY_ATSC     ((UINT16)  3700)  /* kHz */  
#define C_RA231_IF_FREQUENCY_QAM 	  ((UINT16)  3700)/* kHz */  
#define C_RA231_IF_FREQUENCY_NTSC_M   ((UINT16)  5750)  /* kHz */
#define C_RA231_IF_FREQUENCY_ISDBT_6M ((UINT16)  3550)  /* kHz */

//#define RA231_ANA_LO_STEP             ((UINT16) 100)  /* 10Hz */

#define C_RA231_AGC_IF_SLP           -0.12	
#define C_RA231_AGC_IF_INT           0.5   
#define C_RA231_AGC_IF_MAX           0.499  // 0.5 will overflow  //0v
#define C_RA231_AGC_IF_MIN          0.0  

#define C_U8_RA231_AGC_IF_SLP         (INT8)(C_RA231_AGC_IF_SLP *64 )  // MT5135(S(1, -6)), -0.125*64 = -8 (0xF8), -8/64 = -1/8
#define C_U8_RA231_AGC_IF_INT         (INT8)(C_RA231_AGC_IF_INT *16 ) // MT5135(S(3, -4))  0.5*16 = 8, 8/16 =0.5
#define C_U8_RA231_AGC_IF_MAX         (INT8)(C_RA231_AGC_IF_MAX *256)    //0.499*256 = 0x7F
#define C_U8_RA231_AGC_IF_MIN         (INT8)(C_RA231_AGC_IF_MIN *256)    //0

#define C_SONY_RA231_STR	      "SONY_RA231 2013-06-21\n v1.2"		

/* Tuner control data type. */
typedef struct sonyrf_tuner_ctx
{
    union {
        sonyrf_ascot3es1_t tunerEs1;
        sonyrf_ascot3_t tunerEs2;
    } tuner;
    sonyrf_i2c_t     i2c;
    uint8_t          isEs1;
    BOOL fgCable;    /* Analog signal input, 0:Air input, 1:Cable input */
} sonyrf_tuner_ctx;

/* Tuner control data instance. */
static sonyrf_tuner_ctx g_sonyrf_tuner_ctx;

#ifndef SONYRF_DUMMY_DEBUG

static UINT8* pRA231EqNormal[MOD_ANA_TYPE_END - MOD_ANA_TYPE_BEGIN] = {NULL};
static UINT8* pRA231EqWeak[MOD_ANA_TYPE_END - MOD_ANA_TYPE_BEGIN] = {NULL};

//Tuner releated ATD register
static ATD_TUNER_REL_REG_T arrAtdRelReg[]={
#if !(defined(CC_MT5365) || defined(CC_MT5395))
	{0x790,0x08},//DAGC setting
	{0x794,0x30},//DAGC Target level H
	{0x7A4,0x30},//DAGC Target level L
	{0x7B7,0x10},//PRA STATIC TARGET LEVEL HIGH(MSB),degrade target level for lowif tuner(legacy)
    {0x7B9,0x10},//PRA STATIC TARGET LEVEL LOW(MSB)
    {0x7D5,0x07},//PRA IF BIAS 1
    {0x7D6,0x07},//PRA IF BIAS 2   
    {0x7D8,0xF7},//PRA IF SLOPE 1
    {0x7D9,0xF7},//PRA IF SLOPE 2
    {0x7DB,0x7F},//PRA IF AGC MINIMUM
    {0x7DD,0xF2},//PRA IF AGC MINIMUM 1
    {0x7DF,0x51},  //IFAGC PORIRITY
    {0x00,0x00}//end flag
#else
    {0x2B7,0x10},//PRA target H
    {0x2B9,0x10},//PRA target L
    {0x290,0x00},//DAGC setting
    {0x2DF,0x51},//AGC polarity inverse.RF_AGC  disable.IF_AGC polarity inverse
    {0x294,0x38},//DAGC Target level H
    {0x2A4,0x28},//DAGC Target level L
    {0x2CA,0x88},//PRA speed
    {0x00,0x00}//end flag
#endif
};

#endif // SONYRF_DUMMY_DEBUG

//-----------------------------------------------------------------------------
/** RA231_TunerInit
 *  Tuner initialzation for RA231.
 *  @param  pTCtx       Pointer of pi tuner driver context.
 *  @retval void
 */
//-----------------------------------------------------------------------------
VOID RA231_TunerInit(
    ITUNER_CTX_T *pTCtx
    )
{	
    sonyrf_result_t sonyrf_result = SONYRF_RESULT_OK;
    sonyrf_ascot3es1_t *pTunerEs1 = &g_sonyrf_tuner_ctx.tuner.tunerEs1;
    sonyrf_ascot3_t *pTuner = &g_sonyrf_tuner_ctx.tuner.tunerEs2;
    sonyrf_i2c_t *pI2c = &g_sonyrf_tuner_ctx.i2c;
    uint8_t data = 0x00;

#ifndef SONYRF_DUMMY_DEBUG

    ITUNER_CTX_T *psTunerCtx = pTCtx;
	SPECIFIC_MEMBER_US_CTX * pUSCtx= &(pTCtx->specific_member.us_ctx);

	psTunerCtx->I2cAddress = C_RA231_LO_ADDRESS;
    psTunerCtx->u2IF_Freq = C_RA231_IF_FREQUENCY_ATSC;
    psTunerCtx->u2IF_Freq_A = C_RA231_IF_FREQUENCY_NTSC_M;//analog if frequency for ATD
    //ATD patch selection
    psTunerCtx->u1AtdPatchSwitch =0x09;
    
    psTunerCtx->fgRFAGC = FALSE;//indicate if tuner need extern RF_AGC
    psTunerCtx->fgRFTuner =FALSE;//not sawless tuner,it include BPF internal

	// AGC para
     psTunerCtx->sAtdAgcPara.AgcRfBias = 0x2F;     //CJ 091021//0x19;
     psTunerCtx->sAtdAgcPara.AgcRfMin  = 0x80;
     psTunerCtx->sAtdAgcPara.AgcIfMin  = 0xD5;      //CJ 091021//0xE8;//Yueqin's test result 090925 0xEF;According to YueQin's advice 090921
     psTunerCtx->sAtdAgcPara.AgcDrSel  = 0x0B;
	 
	pUSCtx->m_aucPara_SLD[2] = (UINT8)C_U8_RA231_AGC_IF_SLP;
    pUSCtx->m_aucPara_SLD[3] = (UINT8)C_U8_RA231_AGC_IF_INT;
    pUSCtx->m_aucPara_SLD[4] = (UINT8)C_U8_RA231_AGC_IF_MAX;
    pUSCtx->m_aucPara_SLD[5] = (UINT8)C_U8_RA231_AGC_IF_MIN;
	
	pUSCtx->m_s4FreqBoundUpper = C_RA231_FREQ_DBOUND_UPPER;
    pUSCtx->m_s4FreqBoundLower = C_RA231_FREQ_DBOUND_LOWER;
    pUSCtx->m_s4AnaFreqBoundUpper = C_RA231_FREQ_DBOUND_UPPER_Ana;
    pUSCtx->m_s4AnaFreqBoundLower = C_RA231_FREQ_DBOUND_LOWER_Ana;

	pUSCtx->u2LO_Step = 100; //*10HZ
	// init EQ script
	pRA231EqNormal[MOD_NTSC_M - MOD_ANA_TYPE_BEGIN]  = EQRA231M;
	pRA231EqWeak[MOD_NTSC_M - MOD_ANA_TYPE_BEGIN]  = EQRA231MWeak;
    // ...

#endif // SONYRF_DUMMY_DEBUG

    // RA231 driver initialization
    g_sonyrf_tuner_ctx.fgCable = 0;
	mcDBG_MSG_L2((" %s \n", __FUNCTION__));

    // I2C struct initialization
    // NOTE: Please change following line depend on RF tuner I2C.
    //sonyrf_i2c_MTK_CreateI2c(pI2c); // Normal (via I2C repeater)
    #if defined(CC_SONYDEMOD_CXD2838)
    sonyrf_i2c_MTK_CreateI2cGw(pI2c, 0xD8, 0x09); // I2C gateway
    #else
    sonyrf_i2c_MTK_CreateI2c(pI2c); // Normal (via I2C repeater)
    #endif
    // Chip ID check to distingush ES1/ES2.
    pI2c->ReadRegister(pI2c, 0xC0, 0x7F, &data, 1);

    // Tuner struct initialization
    if((data & 0xF0) == 0xC0){
        // ES1
        mcDBG_MSG_L2(("RA231Tuner: WARNING!! Previous version of tuner (ES1) is detected!\n"));
        sonyrf_ascot3es1_ra231_Create(pTunerEs1, pI2c);
        g_sonyrf_tuner_ctx.isEs1 = 1;
    }else if((data & 0xF0) == 0xD0){
        // ES2
        mcDBG_MSG_L2(("RA231Tuner: Latest version of tuner (ES2) is detected.\n"));
        sonyrf_ascot3_ra231_Create(pTuner, pI2c);
        g_sonyrf_tuner_ctx.isEs1 = 0;
    }else{
        mcDBG_MSG_L1(("RA231Tuner: ERROR!! Not RA231 module!\n"));
    }

    if(g_sonyrf_tuner_ctx.isEs1){
        sonyrf_result = sonyrf_ascot3es1_ra231_Initialize(pTunerEs1);
        if(sonyrf_result != SONYRF_RESULT_OK){
            mcDBG_MSG_L2(("RA231Tuner: sonyrf_ascot3es1_ra231_Initialize is failed. (%d)\n", sonyrf_result));
        }
    }else{
        sonyrf_result = sonyrf_ascot3_ra231_Initialize(pTuner);
        if(sonyrf_result != SONYRF_RESULT_OK){
            mcDBG_MSG_L2(("RA231Tuner: sonyrf_ascot3_ra231_Initialize is failed. (%d)\n", sonyrf_result));
        }
    }
}

//----------------------------------------------------------------------------- 
/**  RA231_TunerGetStatus
 *  Set Tuner PLL staus for RA231
 *  @param  pTCtx       Pointer of pi tuner driver context.
 *  @retval TRUE        PLL lock
 *  @retval FALSE       PLL unlock
 */
//-----------------------------------------------------------------------------
static BOOL RA231_TunerGetStatus(ITUNER_CTX_T *pTCtx)
{
    sonyrf_result_t sonyrf_result = SONYRF_RESULT_OK;
    sonyrf_i2c_t *pI2c = &g_sonyrf_tuner_ctx.i2c;

    UINT8 data = 0;
    mcDBG_MSG_L2((" %s \n",__FUNCTION__));

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
/** RA231_TunerSetFreq
 *  Set Tuner PLL for RA231 to tune RF frequency.
 *  @param  pTCtx       Pointer of pi tuner driver context.
 *  @param 	param    Pointer of structure of parameters
 *  @retval  0 success
                    1  fail   out of range 
                   -1  fail  I2C error 
 */
//-----------------------------------------------------------------------------
INT16 RA231_TunerSetFreq(
    ITUNER_CTX_T *pTCtx,
    PARAM_SETFREQ_T* param
    )
{
    sonyrf_result_t sonyrf_result = SONYRF_RESULT_OK;
    sonyrf_ascot3es1_t *pTunerEs1 = &g_sonyrf_tuner_ctx.tuner.tunerEs1;
    sonyrf_ascot3_t *pTuner = &g_sonyrf_tuner_ctx.tuner.tunerEs2;
    sonyrf_ascot3_tv_system_t system = SONYRF_ASCOT3_TV_SYSTEM_UNKNOWN;

    UINT32 Freq = param->Freq; // transfer to video freq(KHz)
    UINT8 Mode = param->Modulation;

    mcDBG_MSG_L2((" %s \n", __FUNCTION__));
    pTCtx->u4RF_Freq = Freq; // Update current frequency to context

    mcDBG_MSG_L2(("RA231Tuner: Set Freq %d KHz\n",Freq));
    if (Freq < (C_RA231_FREQ_DBOUND_LOWER/1000) || 
        Freq > (C_RA231_FREQ_DBOUND_UPPER/1000))
    {
        mcDBG_MSG_L2(("RA231Tuner: Out of range for LO!\n"));
        return 1;
    }

    if(Mode == MOD_ATSC_8VSB){
        mcDBG_MSG_L2(("RA231Tuner: ATSC  SetFreq (%d kHz)\n", Freq));
		pTCtx->u2IF_Freq=C_RA231_IF_FREQUENCY_ATSC;
        system = SONYRF_ASCOT3_DTV_8VSB;
    }
	else if((Mode==MOD_J83B_64QAM)||(Mode==MOD_J83B_256QAM)||(Mode==MOD_J83B_AUTO_QAM)){ 
		mcDBG_MSG_L2(("RA231Tuner: QAM  SetFreq (%d kHz)\n", Freq));
		pTCtx->u2IF_Freq=C_RA231_IF_FREQUENCY_QAM;
        system = SONYRF_ASCOT3_DTV_QAM;
	}
    else if(Mode == MOD_ISDBT){
        mcDBG_MSG_L2(("RA231Tuner: ISDB-T SetFreq (%d kHz)\n", Freq));
		pTCtx->u2IF_Freq=C_RA231_IF_FREQUENCY_ISDBT_6M;
        system = SONYRF_ASCOT3_DTV_ISDBT_6;
    }
	else{
        mcDBG_MSG_L2(("RA231Tuner: Analog SetFreq (%d kHz, fgCable = %d)\n", Freq, g_sonyrf_tuner_ctx.fgCable));
        // Analog mode
        // Assume System-M
		pTCtx->u2IF_Freq_A=C_RA231_IF_FREQUENCY_NTSC_M;
        system = SONYRF_ASCOT3_ATV_MN_SAP;
    }

    if(g_sonyrf_tuner_ctx.isEs1){
        sonyrf_result = sonyrf_ascot3es1_ra231_Tune(pTunerEs1, Freq, (sonyrf_ascot3es1_tv_system_t)system);
        if(sonyrf_result != SONYRF_RESULT_OK){
            mcDBG_MSG_ERR(("sonyrf_ascot3es1_ra231_Tune is Fail!\n"));
            return -1;
        }
    }else{
        sonyrf_result = sonyrf_ascot3_ra231_Tune(pTuner, Freq, system);
        if(sonyrf_result != SONYRF_RESULT_OK){
            mcDBG_MSG_ERR(("sonyrf_ascot3_ra231_Tune is Fail!\n"));
            return -1;
        }
    }

	x_thread_delay(50);

    return 0;
}

//----------------------------------------------------------------------------- 
/** RA231_TunerGetVer
 *  Get Tuner type version
 *  @param  pTCtx       Pointer of pi tuner driver context.
 *  @retval tuner model name
 */
//-----------------------------------------------------------------------------
static CHAR *RA231_TunerGetVer(VOID)
{
    return (C_SONY_RA231_STR);
}

//----------------------------------------------------------------------------- 
/** RA231_SetInMode
 *  Set Tuner for Cable or Air input mode
 *  @param  pTunerCtx       Pointer of pi tuner driver context.
 *  @param  bMode           Mode (0: Air, 1: Cable)
 *  @retval void
 */
//-----------------------------------------------------------------------------
static void RA231_SetInMode(ITUNER_CTX_T * pTunerCtx, BOOL bMode)
{
    g_sonyrf_tuner_ctx.fgCable = bMode;
    mcDBG_MSG_L2(("RA231Tuner: In Mode = %d (0: Air, 1:Cable)\n", g_sonyrf_tuner_ctx.fgCable));  
}

//----------------------------------------------------------------------------- 
/** RA231_GetRSSI
 *  Get RSSI value in dB x 100
 *  @retval INT32 RSSI value
 */
//-----------------------------------------------------------------------------
static INT32 RA231_GetRSSI(VOID)
{
    sonyrf_ascot3es1_t *pTunerEs1 = &g_sonyrf_tuner_ctx.tuner.tunerEs1;
    sonyrf_ascot3_t *pTuner = &g_sonyrf_tuner_ctx.tuner.tunerEs2;
    sonyrf_result_t sonyrf_result = SONYRF_RESULT_OK;
    int32_t rssi_x100 = 0;

    if(g_sonyrf_tuner_ctx.isEs1){
        sonyrf_result = sonyrf_ascot3es1_ra231_RSSI(pTunerEs1, &rssi_x100);
    }else{
        sonyrf_result = sonyrf_ascot3_ra231_RSSI(pTuner, &rssi_x100);
    }
    if(sonyrf_result != SONYRF_RESULT_OK){
        rssi_x100 = 0;
    }
    mcDBG_MSG_L2(("RA231Tuner: RSSI (dB x 100) = %d\n", rssi_x100));

    return (INT32)rssi_x100;
}

//----------------------------------------------------------------------------- 
/** RA231_TunerOP
 * Set/Get function
 *  @param  pTCtx       Pointer of pi tuner driver context.
 *  @param  eOperation       To Get/Set operation id
 *  @retval  ITUNER_OK    Operation Success
 *  @retval  ITUNER_NOT_OK    Operation fail
 *  @retval  ITUNER_NOT_SUPPORTED Operation not supported in  driver
 */
//-----------------------------------------------------------------------------
INT16 RA231_TunerOP(ITUNER_CTX_T * pTCtx, ITUNEROP_T eOperation, UINT8 SetVal,  VOID * pInOutVal)
{
    switch(eOperation){
    case itGetStatus:
        {
            BOOL * Retval=(BOOL *)pInOutVal;
            *Retval = RA231_TunerGetStatus(pTCtx);
        }
        break;
    case itGetVer:
        {
            CHAR ** RetStr=(CHAR **)pInOutVal;
            *RetStr = RA231_TunerGetVer();
        }
        break;
    case itSetInCableMode: 
        RA231_SetInMode(pTCtx,SetVal);
        break;

#ifndef SONYRF_DUMMY_DEBUG

    case itGetEqScriptNormal: 
        *(UINT8**)pInOutVal = pRA231EqNormal[SetVal];
        break;
        
    case itGetEqScriptWeak: 
        *(UINT8**)pInOutVal = pRA231EqWeak[SetVal];
        break;
    case itGetTunerRelReg:
        {
            *(ATD_TUNER_REL_REG_T **)pInOutVal=arrAtdRelReg;
        }
        break;

#endif // SONYRF_DUMMY_DEBUG

//Xiaohong Add for Sony Signal strengh get 20131025
case itGetSSIIndex:
	{
		INT32 * Retval = (INT32*)pInOutVal;
   INT32  Rssi =0;
	 //*Retval = RA231_GetRSSI()/100+108+20;
		Rssi = RA231_GetRSSI()/100+108+20-38;
	  if(Rssi > 91)
		  {
			 *Retval= 100;
		 }
	  else if(Rssi < 5)
		 {
			 *Retval= 0;
		 }
	 else
		 {
			*Retval = RA231_GetRSSI()/100+108+20-38;
		 }
	}
	break;

    case itSetTrapThresold:
        {
            // Not implemented for now
            mcDBG_MSG_L2(("RA231Tuner: Not supported.\n"));

            /*
            UINT32 threshold = *(UINT32*)pInOutVal;
            if(SetVal == 0){ // Digital
                // Change digital threshold
                g_TrapThresholdFrequencyDigital = threshold;
                mcDBG_MSG_L2(("RA231Tuner: Trap threshold (digital) = %d\n", threshold));
            }else{ // Analog
                // Change analog threshold
                g_TrapThresholdFrequencyAnalog = threshold;
                mcDBG_MSG_L2(("RA231Tuner: Trap threshold (analog) = %d\n", threshold));
            }
            */
        }
        break;
    default:
        return ITUNER_NOT_SUPPORTED;
    }

    return ITUNER_OK;
}

/* For driver debug trace --------------------------------------------------- */
#ifdef SONYRF_TRACE_ENABLE
void sonyrf_trace_log_enter(const char* funcname, const char* filename, unsigned int linenum)
{
    mcDBG_MSG_L4(("RA231_ENTER : %s (%d)\n", funcname, linenum));
}
void sonyrf_trace_log_return(sonyrf_result_t result, const char* filename, unsigned int linenum)
{
    if(result != SONYRF_RESULT_OK){
        mcDBG_MSG_L4(("RA231_ERROR_RETURN(%d) : (line %d)\n", result, linenum));
    }else{
        mcDBG_MSG_L4(("RA231_RETURN : (line %d))\n", linenum));
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
